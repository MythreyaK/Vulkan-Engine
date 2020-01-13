#ifndef RENDER_SWAPCHAIN_HPP
#define RENDER_SWAPCHAIN_HPP

#include "VKinclude/VKinclude.hpp"
#include <vector>

namespace Engine::Render::Device {
    class PhysicalDevice;
}

namespace Engine::Render::Swapchain {

    vk::UniqueSwapchainKHR      CreateSwapchain(const vk::Device&, const Engine::Render::Device::PhysicalDevice&, const vk::SurfaceKHR&);
    vk::UniqueSwapchainKHR      CreateSwapchain(const vk::Device&, const Engine::Render::Device::PhysicalDevice&, const vk::SurfaceKHR&, vk::SwapchainKHR& oldChain);
    vk::SwapchainCreateInfoKHR  GetSwapchainCreateInfo(const Engine::Render::Device::PhysicalDevice&, const vk::SurfaceKHR& surface);



    std::vector<vk::Image>              GetSwapchainImages(const vk::Device& renderDevice, const vk::SwapchainKHR& swapchain);
    std::vector<vk::UniqueImageView>    CreateImageViews(const vk::Device& renderDevice, const Engine::Render::Device::PhysicalDevice& devInf, const std::vector<vk::Image>& swapImages);
    std::vector<vk::UniqueFramebuffer>  CreateFramebuffers(const vk::Device& renderDevice, const vk::RenderPass& renderPass, const std::vector<vk::UniqueImageView>& swapImageViews, const vk::Extent2D&);

}


#endif // !RENDER_SWAPCHAIN_HPP
