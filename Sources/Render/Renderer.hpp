#ifndef RENDER_RENDERER_HPP

#define RENDER_RENDERER_HPP

#include "VKinclude/VKinclude.hpp"
#include "Device/Physical.hpp"
#include "Pipeline/Pipeline.hpp"

struct GLFWwindow;
typedef GLFWwindow WindowHandle;

namespace Engine::Render {

    namespace ERD = Engine::Render::Device;

    class Renderer {

    private:
        using UniqueDebugMessenger  = vk::UniqueDebugUtilsMessengerEXT;
        using UniqueImageViews      = std::vector<vk::UniqueImageView>;
        using UniqueFramebuffers    = std::vector<vk::UniqueFramebuffer>;
        using UniqueCommandBuffers  = std::vector<vk::UniqueCommandBuffer>;
        using UniqueImagesSemaphore = std::vector<vk::UniqueSemaphore>;
        using UniqueRenderSemaphore = std::vector<vk::UniqueSemaphore>;
        using UniqueImageFences     = std::vector<vk::UniqueFence>;
        using UniqueRenderFences    = std::vector<vk::UniqueFence>;

        vk::UniqueInstance          renderInstance;
        UniqueDebugMessenger        debugMessenger;
        vk::UniqueSurfaceKHR        renderSurface;
        ERD::PhysicalDevice         physicalDeviceInfo;
        vk::UniqueDevice            renderDevice;
        std::vector<vk::Queue>      queues;
        vk::UniqueSwapchainKHR      swapchain;
        std::vector<vk::Image>      swapImages;
        UniqueImageViews            swapImageViews;
        vk::UniqueRenderPass        renderPass;
        Pipeline                    renderPipeline;
        UniqueFramebuffers          framebuffers;
        vk::UniqueCommandPool       commandPool;
        UniqueCommandBuffers        commandBuffers;
        UniqueImagesSemaphore       imageAvailableSemaphores;
        UniqueRenderSemaphore       renderFinishedSemaphores;
        UniqueImageFences           inFlightFences;

        const int MAX_FRAMES_IN_FLIGHT{ static_cast<int>(swapImages.size()) };
        // No copies!
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        void CreateSyncObjects();
        void DestroySyncObjects();
        void RecreateSwapchain();
        void CleanupSwapchain();

    public:
        Renderer() = default;
        Renderer(const std::vector<const char*>& instanceExtensions, WindowHandle* handle);

        Renderer(Renderer&&)            = default;
        Renderer& operator=(Renderer&&) = default;
        ~Renderer()                     = default;

        void DrawFrame();

        void WaitDevice();

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
