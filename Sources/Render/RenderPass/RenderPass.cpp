#include "RenderPass.hpp"
#include "Device/Physical.hpp"

namespace Engine::Render::RenderPass {

    vk::UniqueRenderPass CreateRenderPass(const vk::Device& device, const ERD::PhysicalDevice& devInfo) {
        const auto attachmentDescription{ vk::AttachmentDescription()
            .setFormat(devInfo.SurfaceFormat().format)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
        };

        const auto attachmentReference{ vk::AttachmentReference()
            .setAttachment(0)
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
        };

        const auto subpass{ vk::SubpassDescription()
            .setColorAttachmentCount(1)
            .setPColorAttachments(&attachmentReference)
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        };

        const auto subpassDependency{ vk::SubpassDependency()
            .setDstSubpass(0)
            .setSrcSubpass(VK_SUBPASS_EXTERNAL)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setSrcAccessMask(vk::AccessFlags())
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite)
        };

        const auto renderpassCreateInfo{ vk::RenderPassCreateInfo()
            .setAttachmentCount(1)
            .setPAttachments(&attachmentDescription)
            .setSubpassCount(1)
            .setPSubpasses(&subpass)
            .setDependencyCount(1)
            .setPDependencies(&subpassDependency)
        };

        return device.createRenderPassUnique(renderpassCreateInfo);
    }

}
