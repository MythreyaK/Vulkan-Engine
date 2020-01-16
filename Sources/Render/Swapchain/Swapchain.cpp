#include "Swapchain.hpp"
#include "Device/Physical.hpp"

namespace Engine::Render::Swapchain {

    namespace ERD = Engine::Render::Device;

    vk::UniqueSwapchainKHR CreateSwapchain(const vk::Device& renderDevice, const ERD::PhysicalDevice& devInf, const vk::SurfaceKHR& surface) {
        return renderDevice.createSwapchainKHRUnique(GetSwapchainCreateInfo(devInf, surface).setOldSwapchain(nullptr));
    }


    vk::UniqueSwapchainKHR CreateSwapchain(const vk::Device& renderDevice, const ERD::PhysicalDevice& devInf, const vk::SurfaceKHR& surface, vk::SwapchainKHR& oldChain) {
        return renderDevice.createSwapchainKHRUnique(GetSwapchainCreateInfo(devInf, surface).setOldSwapchain(oldChain));
    }

    std::vector<vk::Image> GetSwapchainImages(const vk::Device& renderDevice, const vk::SwapchainKHR& swapchain) {
        return renderDevice.getSwapchainImagesKHR(swapchain);
    }

    std::vector<vk::UniqueImageView> CreateImageViews(const vk::Device& renderDevice, const ERD::PhysicalDevice& devInf, const std::vector<vk::Image>& swapImages) {

        std::vector<vk::UniqueImageView> swapImageViews{};

        auto imViewCrInf{ vk::ImageViewCreateInfo()
            .setFormat(devInf.SurfaceFormat().format)
            .setViewType(vk::ImageViewType::e2D)
            .setComponents(vk::ComponentSwizzle::eIdentity)
            .setSubresourceRange(vk::ImageSubresourceRange()
                .setLayerCount(1)
                .setLevelCount(1)
                .setBaseMipLevel(0)
                .setBaseArrayLayer(0)
                .setAspectMask(vk::ImageAspectFlagBits::eColor)
            )
        };

        for (const auto& i : swapImages) {
            swapImageViews.emplace_back(renderDevice.createImageViewUnique(imViewCrInf.setImage(i)));
        }

        return swapImageViews;
    }

    vk::SwapchainCreateInfoKHR GetSwapchainCreateInfo(const ERD::PhysicalDevice& devInf, const vk::SurfaceKHR& surface) {

        const auto capabs       { devInf.Get().getSurfaceCapabilitiesKHR(surface) };
        const auto imageCount   { capabs.minImageCount + 2 };

        const auto swpInfo{ vk::SwapchainCreateInfoKHR()
            .setImageArrayLayers(1)
            .setImageExtent(capabs.currentExtent)
            .setMinImageCount(imageCount)
            .setImageFormat(devInf.SurfaceFormat().format)
            .setImageColorSpace(devInf.SurfaceFormat().colorSpace)
            .setImageSharingMode(vk::SharingMode::eExclusive)
            // TODO: use eTransferDst + memory OP to allow for post processing
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
            .setPreTransform(capabs.currentTransform)
            .setSurface(surface)
            // TODO: Handle the case when we use 2 different queues
            //.setQueueFamilyIndexCount()
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setClipped(true)
            .setPresentMode(devInf.PresentMode())
        };

        return swpInfo;
    }

    std::vector<vk::UniqueFramebuffer> CreateFramebuffers(const vk::Device& renderDevice, const vk::RenderPass& renderPass, const std::vector<vk::UniqueImageView>& swapImageViews, const vk::Extent2D& swapExtents) {

        std::vector<vk::UniqueFramebuffer> frameBuffers;

        auto framebufferCreateInfo { vk::FramebufferCreateInfo()
            .setRenderPass(renderPass)
            .setHeight(swapExtents.height)
            .setWidth(swapExtents.width)
            .setLayers(1)
        };

        for (const auto& i : swapImageViews) {

            frameBuffers.emplace_back(renderDevice.createFramebufferUnique(framebufferCreateInfo
                .setAttachmentCount(1)
                .setPAttachments(&i.get())
            ));
        }

        return frameBuffers;
    }
}
