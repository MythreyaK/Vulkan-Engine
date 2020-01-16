#include "Renderer.hpp"

#include "Instance/Instance.hpp"
#include "VulkanDebug/DebugUtils.hpp"
#include "Surface/Surface.hpp"
#include "Device/Physical.hpp"
#include "Device/Logical.hpp"
#include "Swapchain/Swapchain.hpp"
#include "Shader/Shader.hpp"
#include "RenderPass/RenderPass.hpp"
#include "Pipeline/Pipeline.hpp"
#include "Command/Command.hpp"
#include "Logger.hpp"

#include <iostream>
#include <set>

namespace Engine::Render {

    namespace ERI   = Engine::Render::Instance;
    namespace ERDB  = Engine::Render::Debug;
    namespace ERS   = Engine::Render::Surface;
    namespace ERD   = Engine::Render::Device;
    namespace ERDL  = Engine::Render::Device::Logical;
    namespace ERSP  = Engine::Render::Swapchain;
    namespace ERRP  = Engine::Render::RenderPass;
    namespace ERCD  = Engine::Render::Command;


    const char GetLevel(const vk::DebugUtilsMessageSeverityFlagBitsEXT& flags);
    const std::string GetType(const vk::DebugUtilsMessageTypeFlagsEXT& fl);

    Renderer::Renderer(const std::vector<const char*>& instanceExtensions, WindowHandle* handle) :
        renderInstance      (ERI::CreateInstance        (instanceExtensions,    std::nullopt,                       handle  )),
#       ifdef BUILD_TYPE_DEBUG
        debugMessenger      (ERDB::CreateDebugMessenger (renderInstance.get(),  debug_callback,                     this    )),
#       endif // BUILD_TYPE_DEBUG
        renderSurface       (ERS::CreateSurface         (renderInstance.get(),  handle                                      )),
        physicalDeviceInfo  (ERD::PickDevice            (renderInstance.get(),  renderSurface.get()                         )),
        renderDevice        (ERDL::CreateLogicalDevice  (physicalDeviceInfo,    renderSurface.get()                         )),
        queues              (ERDL::GetQueues            (renderDevice.get(),    physicalDeviceInfo                          )),
        swapchain           (ERSP::CreateSwapchain      (renderDevice.get(),    physicalDeviceInfo,                 renderSurface.get())),
        swapImages          (ERSP::GetSwapchainImages   (renderDevice.get(),    swapchain.get()         )),
        swapImageViews      (ERSP::CreateImageViews     (renderDevice.get(),    physicalDeviceInfo,                 swapImages              )),
        renderPass          (ERRP::CreateRenderPass     (renderDevice.get(),    physicalDeviceInfo      )),
        renderPipeline      (Pipeline                   (renderDevice.get(),    renderPass.get(),                   physicalDeviceInfo.GetExtent2D(renderSurface.get())    )),
        framebuffers        (ERSP::CreateFramebuffers   (renderDevice.get(),    renderPass.get(),                   swapImageViews,                                         physicalDeviceInfo.GetExtent2D(renderSurface.get())  )),
        commandPool         (ERCD::CreateQueueCommandPool(renderDevice.get(),   physicalDeviceInfo.PresentQueue()                           )),
        commandBuffers      (ERCD::CreateCommandBuffer  (renderDevice.get(),    commandPool.get(),                  swapImageViews.size()   ))
    {
        ERCD::RecordCommands(commandBuffers, framebuffers, renderPass.get(), renderPipeline, physicalDeviceInfo.GetExtent2D(renderSurface.get()));
        CreateSyncObjects();
    }


    void Renderer::CreateSyncObjects() {

        for (int i = 0; i < GetMaxFramesInFlight(); i++) {
            imageAvailableSemaphores.emplace_back(renderDevice->createSemaphoreUnique({}));
            renderFinishedSemaphores.emplace_back(renderDevice->createSemaphoreUnique({}));

            inFlightFences.emplace_back(renderDevice->createFenceUnique(vk::FenceCreateInfo()
                .setFlags(vk::FenceCreateFlagBits::eSignaled)
            ));
        }
    }

    void Renderer::DestroySyncObjects() {
        imageAvailableSemaphores.~vector();
        renderFinishedSemaphores.~vector();
        inFlightFences.~vector();
    }

    void Renderer::DrawFrame() {

        static int currentFrame{ 0 };
        static std::vector<bool> imagesInFlight(GetMaxFramesInFlight());

        renderDevice->waitForFences(1, &inFlightFences[currentFrame].get(), true, UINT64_MAX);

        vk::ResultValue<uint32_t> acquireResult{ vk::Result{}, 0 };

        try {
            acquireResult = renderDevice->acquireNextImageKHR(swapchain.get(), UINT64_MAX, imageAvailableSemaphores[currentFrame].get(), nullptr);
        }
        catch (const std::exception&) {
            queues[0].waitIdle();
            DestroySyncObjects();
            WaitDevice();
            CleanupSwapchain();
            RecreateSwapchain();
            CreateSyncObjects();
            return;
        }

        const auto imageIndex{ acquireResult.value};

        if (imagesInFlight[imageIndex]) {
            renderDevice->waitForFences(1, &inFlightFences[imageIndex].get(), true, UINT64_MAX);
        }

        imagesInFlight[currentFrame] = true;

        const auto stageMask { vk::PipelineStageFlags() |
            vk::PipelineStageFlagBits::eColorAttachmentOutput };

        const auto submitInfo{ vk::SubmitInfo()
            .setWaitSemaphoreCount(1)
            .setCommandBufferCount(1)
            .setPCommandBuffers(&commandBuffers[imageIndex].get())
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&imageAvailableSemaphores[currentFrame].get())
            .setPWaitDstStageMask(&stageMask)
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&renderFinishedSemaphores[currentFrame].get())
        };

        renderDevice->resetFences(1, &inFlightFences[currentFrame].get());
        queues[0].submit(submitInfo, inFlightFences[currentFrame].get());

        const auto presentInfo { vk::PresentInfoKHR()
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&renderFinishedSemaphores[currentFrame].get())
            .setPSwapchains(&swapchain.get())
            .setSwapchainCount(1)
            .setPImageIndices(&imageIndex)
        };

        try {
            queues[0].presentKHR(presentInfo);
        }

        catch (const std::exception&) {
            queues[0].waitIdle();
            DestroySyncObjects();
            WaitDevice();
            CleanupSwapchain();
            RecreateSwapchain();
            CreateSyncObjects();
            return;
        }

        // TODO: Disable Vulkan exceptions and use if/else

        currentFrame = (currentFrame + 1) % GetMaxFramesInFlight();
    }


    void Renderer::WaitDevice() {
        renderDevice->waitIdle();
    }


    void Renderer::RecreateSwapchain() {
        swapchain       = ERSP::CreateSwapchain(renderDevice.get(), physicalDeviceInfo, renderSurface.get(), swapchain.get());
        swapImages      = ERSP::GetSwapchainImages(renderDevice.get(), swapchain.get());
        swapImageViews  = ERSP::CreateImageViews(renderDevice.get(), physicalDeviceInfo, swapImages);
        renderPass      = ERRP::CreateRenderPass(renderDevice.get(), physicalDeviceInfo);
        renderPipeline  = Pipeline(renderDevice.get(), renderPass.get(), physicalDeviceInfo.GetExtent2D(renderSurface.get()));
        framebuffers    = ERSP::CreateFramebuffers(renderDevice.get(), renderPass.get(), swapImageViews, physicalDeviceInfo.GetExtent2D(renderSurface.get()));
        commandPool     = ERCD::CreateQueueCommandPool(renderDevice.get(), physicalDeviceInfo.PresentQueue());
        commandBuffers  = ERCD::CreateCommandBuffer(renderDevice.get(), commandPool.get(), swapImageViews.size());

        ERCD::RecordCommands(commandBuffers, framebuffers, renderPass.get(), renderPipeline, physicalDeviceInfo.GetExtent2D(renderSurface.get()));
    }


    void Renderer::CleanupSwapchain() {
        commandBuffers.~vector();
        commandPool.reset();
        framebuffers.~vector();
        renderPass.reset();
        swapImageViews.~vector();
        swapImages.~vector();
        swapchain.reset();
    }

    const int Renderer::GetMaxFramesInFlight() {
        assert(swapImages.size() > 0 || 0 == "Swap Images size is zero!");
        return swapImages.size();
    }


    void Renderer::ValidationMessageCallback(
        const vk::DebugUtilsMessageSeverityFlagBitsEXT& messageSeverity,
        const vk::DebugUtilsMessageTypeFlagsEXT&        messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT&   callbackData) {

        LOGGER << "[VAL "
            << GetLevel(messageSeverity) << ": "
            << GetType(messageType) << " ]"
            << callbackData.pMessage << "\n";
        //callbackData.
    }


    const char GetLevel(const vk::DebugUtilsMessageSeverityFlagBitsEXT& flags) {

        using VK_Flg = vk::DebugUtilsMessageSeverityFlagBitsEXT;

        switch (flags) {
        case VK_Flg::eVerbose:
            return 'V';

        case VK_Flg::eInfo:
            return 'I';

        case VK_Flg::eWarning:
            return 'W';

        case VK_Flg::eError:
            return 'E';

        default:
            return '*';
        }
    }

    const std::string GetType(const vk::DebugUtilsMessageTypeFlagsEXT& fl) {
        using VK_Flg = vk::DebugUtilsMessageTypeFlagBitsEXT;

        std::string ret{ "   " };

        if (fl & VK_Flg::eValidation)   ret[0] = 'V';
        if (fl & VK_Flg::ePerformance)  ret[1] = 'P';
        if (fl & VK_Flg::eGeneral)      ret[2] = 'G';

        return ret;
    }

    VkBool32 VKAPI_PTR debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void*                                       pUserData
    ) {
        reinterpret_cast<Renderer*>(pUserData)->ValidationMessageCallback(
            static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity),
            static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes),
            static_cast<vk::DebugUtilsMessengerCallbackDataEXT>(*pCallbackData)
        );

        return VK_FALSE;
    }

}
