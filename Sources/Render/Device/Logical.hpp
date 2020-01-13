#ifndef RENDER_LOGICAL_DEVICE_HPP

#define RENDER_LOGICAL_DEVICE_HPP

#include "VKinclude/VKinclude.hpp"

namespace Engine::Render::Device {
    class PhysicalDevice;
}

namespace Engine::Render::Device::Logical {

    using ch_vec = std::vector<const char*>;

    vk::UniqueDevice CreateLogicalDevice(const PhysicalDevice&, vk::SurfaceKHR&);
    std::vector<vk::Queue> GetQueues(const vk::Device& renderDevice, const PhysicalDevice&);


    //using SwapImages        = std::vector<vk::Image>;
    //using OptionalQueue     = std::optional<vk::Queue>;
    //using USwapImageViews   = std::vector<vk::UniqueImageView>;


    //void CreateImageViews();
    //const vk::Extent2D GetImageExtents();
    //vk::UniqueSwapchainKHR CreateSwapchain(const ERD_Phy::DeviceInfo& dev_inf);
    //vk::UniqueSwapchainKHR CreateSwapchain(const ERD_Phy::DeviceInfo& dev_inf, vk::SwapchainKHR& old_chain);
    //vk::SwapchainCreateInfoKHR GetSwapchainCreateInfo(const ERD_Phy::DeviceInfo& dev_inf);
    //vk::Queue& GetGraphicsQueue();
    //vk::Queue& GetPresentQueue();
}

#endif // !RENDER_LOGICAL_DEVICE_HPP
