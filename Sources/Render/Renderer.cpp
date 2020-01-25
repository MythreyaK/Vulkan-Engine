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
    namespace ERQU  = Engine::Render::Queue;
    namespace ERSP  = Engine::Render::Swapchain;
    namespace ERRP  = Engine::Render::RenderPass;
    namespace ERCD  = Engine::Render::Command;

    auto ERQUG = ERQU::QueueType::Graphics;

    const char GetLevel(const vk::DebugUtilsMessageSeverityFlagBitsEXT& flags);
    const std::string GetType(const vk::DebugUtilsMessageTypeFlagsEXT& fl);
    const std::map<ERQU::QueueType, int> GetNeededQueues();

    Renderer::Renderer(const std::vector<const char*>& instanceExtensions, WindowHandle* handle) :

        renderInstance  (ERI::CreateInstance          (instanceExtensions,    std::nullopt,          handle )),
#       ifdef BUILD_TYPE_DEBUG                                                                        
        debugMessenger  (ERDB::CreateDebugMessenger   (renderInstance.get(),  debug_callback,        this   )),
#       endif // BUILD_TYPE_DEBUG                                                                     
        renderSurface   (ERS::CreateSurface           (renderInstance.get(),  handle                 )),
        deviceInfo      (ERD::PickDevice              (renderInstance.get(),  renderSurface.get()           )),
        queues          (ERQU::QueueManager           (deviceInfo.Get(),      renderSurface.get(),   GetNeededQueues()   )),
        renderDevice    (ERDL::CreateLogicalDevice    (deviceInfo,            renderSurface.get(),   queues              )),
        swapchain       (ERSP::CreateSwapchain        (renderDevice.get(),    deviceInfo,            renderSurface.get() )),
        swapImages      (ERSP::GetSwapchainImages     (renderDevice.get(),    swapchain.get()                            )),
        swapImageViews  (ERSP::CreateImageViews       (renderDevice.get(),    deviceInfo,            swapImages          )),
        renderPass      (ERRP::CreateRenderPass       (renderDevice.get(),    deviceInfo                                 )),
        renderPipeline  (Pipeline                     (renderDevice.get(),    renderPass.get(),      deviceInfo.GetExtent2D(renderSurface.get()) )),
        framebuffers    (ERSP::CreateFramebuffers     (renderDevice.get(),    renderPass.get(),      swapImageViews,                             deviceInfo.GetExtent2D(renderSurface.get())  )),
        commandPools    (ERCD::CreateQueueCommandPool (renderDevice.get(),    queues                                     )),
        commandBuffers  (ERCD::CreateCommandBuffers   (renderDevice.get(),    commandPools,          swapImageViews.size()))
    {
        ERCD::RecordCommands(ERQU::QueueType::Graphics, commandBuffers[ERQU::QueueType::Graphics], framebuffers, renderPass.get(), renderPipeline, deviceInfo.GetExtent2D(renderSurface.get()));
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
        imageAvailableSemaphores.clear();
        renderFinishedSemaphores.clear();
        inFlightFences.clear();
    }

    void Renderer::DrawFrame() {

        static int currentFrame{ 0 };

        vk::ResultValue<uint32_t> acquireResult{ vk::Result{}, 0 };

        try {
            acquireResult = renderDevice->acquireNextImageKHR(swapchain.get(), UINT64_MAX, imageAvailableSemaphores[currentFrame].get(), nullptr);
            renderDevice->waitForFences(1, &inFlightFences[acquireResult.value].get(), true, UINT64_MAX);
        }
        catch (const std::exception&) {
            queues[ERQU::QueueType::Graphics].waitIdle();
            DestroySyncObjects();
            WaitDevice();
            CleanupSwapchain();
            RecreateSwapchain();
            CreateSyncObjects();
            return;
        }

        const auto imageIndex{ acquireResult.value };

        static const auto stageMask { vk::PipelineStageFlags() |
            vk::PipelineStageFlagBits::eColorAttachmentOutput };

        const auto submitInfo{ vk::SubmitInfo()
            .setCommandBufferCount(1)
            .setPCommandBuffers(&commandBuffers[ERQU::QueueType::Graphics][imageIndex].get())
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&imageAvailableSemaphores[currentFrame].get())
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&renderFinishedSemaphores[currentFrame].get())
            .setPWaitDstStageMask(&stageMask)
        };

        renderDevice->resetFences(1, &inFlightFences[currentFrame].get());
        queues[ERQU::QueueType::Graphics].submit(submitInfo, inFlightFences[currentFrame].get());

        const auto presentInfo { vk::PresentInfoKHR()
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&renderFinishedSemaphores[currentFrame].get())
            .setSwapchainCount(1)
            .setPSwapchains(&swapchain.get())
            .setPImageIndices(&imageIndex)
        };

        try {
            queues[ERQU::QueueType::Graphics].presentKHR(presentInfo);
        }
        catch (const std::exception&) {
            queues[ERQU::QueueType::Graphics].waitIdle();
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


    const std::map<ERQU::QueueType, int> GetNeededQueues() {
        return {
            { ERQU::QueueType::Graphics, 1 }
        };
    }


    void Renderer::WaitDevice() {
        renderDevice->waitIdle();
    }



    void Renderer::RecreateSwapchain() {
        swapchain       = ERSP::CreateSwapchain(renderDevice.get(), deviceInfo, renderSurface.get(), swapchain.get());
        swapImages      = ERSP::GetSwapchainImages(renderDevice.get(), swapchain.get());
        swapImageViews  = ERSP::CreateImageViews(renderDevice.get(), deviceInfo, swapImages);
        renderPass      = ERRP::CreateRenderPass(renderDevice.get(), deviceInfo);
        renderPipeline  = Pipeline(renderDevice.get(), renderPass.get(), deviceInfo.GetExtent2D(renderSurface.get()));
        framebuffers    = ERSP::CreateFramebuffers(renderDevice.get(), renderPass.get(), swapImageViews, deviceInfo.GetExtent2D(renderSurface.get()));
        commandPools    = ERCD::CreateQueueCommandPool(renderDevice.get(), queues);
        commandBuffers  = ERCD::CreateCommandBuffers(renderDevice.get(), commandPools, swapImageViews.size());
        ERCD::RecordCommands(ERQU::QueueType::Graphics, commandBuffers[ERQU::QueueType::Graphics], framebuffers, renderPass.get(), renderPipeline, deviceInfo.GetExtent2D(renderSurface.get()));
    }


    void Renderer::CleanupSwapchain() {
        commandBuffers.clear();
        commandPools.clear();
        framebuffers.clear();
        renderPass.reset();
        swapImageViews.clear();
        swapImages.clear();
        swapchain.reset();
    }

    const int Renderer::GetMaxFramesInFlight() {
        assert(swapImages.size() > 0 || 0 == "Swap Images size is zero!");
        return static_cast<int>(swapImages.size());
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
