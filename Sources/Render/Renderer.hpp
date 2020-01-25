#ifndef RENDER_RENDERER_HPP

#define RENDER_RENDERER_HPP

#include "VKinclude/VKinclude.hpp"
#include "Device/Physical.hpp"
#include "Pipeline/Pipeline.hpp"
#include "Queue/Queue.hpp"
#include "Version.hpp"


struct GLFWwindow;
typedef GLFWwindow WindowHandle;

namespace Engine::Render {

    namespace ERD = Engine::Render::Device;
    namespace ERQU = Engine::Render::Queue;

    class Renderer {

    private:
        using UniqueDebugMessenger  = vk::UniqueDebugUtilsMessengerEXT;
        using UniqueImageViews      = std::vector<vk::UniqueImageView>;
        using UniqueFramebuffers    = std::vector<vk::UniqueFramebuffer>;
        using UniqueCommandPools    = std::map<ERQU::QueueType, vk::UniqueCommandPool>;
        using UniqueCommandBuffers  = std::map<ERQU::QueueType, std::vector<vk::UniqueCommandBuffer>>;
        using UniqueImagesSemaphore = std::vector<vk::UniqueSemaphore>;
        using UniqueRenderSemaphore = std::vector<vk::UniqueSemaphore>;
        using UniqueImageFences     = std::vector<vk::UniqueFence>;
        using UniqueRenderFences    = std::vector<vk::UniqueFence>;

        vk::UniqueInstance          renderInstance;
#       ifdef BUILD_TYPE_DEBUG
        UniqueDebugMessenger        debugMessenger;
#       endif // !BUILD_TYPE_DEBUG
        vk::UniqueSurfaceKHR        renderSurface;
        ERD::PhysicalDevice         deviceInfo;
        ERQU::QueueManager          queues;
        vk::UniqueDevice            renderDevice;
        vk::UniqueSwapchainKHR      swapchain;
        std::vector<vk::Image>      swapImages;
        UniqueImageViews            swapImageViews;
        vk::UniqueRenderPass        renderPass;
        Pipeline                    renderPipeline;
        UniqueFramebuffers          framebuffers;
        UniqueCommandPools          commandPools;
        UniqueCommandBuffers        commandBuffers;
        UniqueImagesSemaphore       imageAvailableSemaphores;
        UniqueRenderSemaphore       renderFinishedSemaphores;
        UniqueImageFences           inFlightFences;


        // No copies!
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        void CreateSyncObjects();
        void DestroySyncObjects();
        void RecreateSwapchain();
        void CleanupSwapchain();
        void ReInit();

        const int GetMaxFramesInFlight();

    public:
        Renderer(const std::vector<const char*>& instanceExtensions, WindowHandle* handle);

        Renderer(Renderer&&)            = default;
        Renderer& operator=(Renderer&&) = default;
        ~Renderer()                     = default;

        void DrawFrame();
        void WaitDevice();
        void SuspendRendering();
        void ResumeRendering();

        void ValidationMessageCallback(
            const vk::DebugUtilsMessageSeverityFlagBitsEXT& messageSeverity,
            const vk::DebugUtilsMessageTypeFlagsEXT&        messageType,
            const vk::DebugUtilsMessengerCallbackDataEXT&   callbackData
        );


    };

    VkBool32 VKAPI_PTR debug_callback (
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void*                                       pUserData
    );
}


#endif // !RENDER_RENDERER_HPP
