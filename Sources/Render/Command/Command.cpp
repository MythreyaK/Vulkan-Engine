#include "Command.hpp"
#include "Pipeline\Pipeline.hpp"


namespace Engine::Render::Command {

    vk::UniqueCommandPool CreateQueueCommandPool(const vk::Device& renderDevice, const uint32_t queueFamily) {
        return renderDevice.createCommandPoolUnique(vk::CommandPoolCreateInfo()
            .setQueueFamilyIndex(queueFamily)
        );

    }

    std::vector<vk::UniqueCommandBuffer> CreateCommandBuffer(const vk::Device& renderDevice, const vk::CommandPool& cmdPool, const uint32_t numBuffers) {
        return renderDevice.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo()
            .setCommandPool(cmdPool)
            .setCommandBufferCount(numBuffers)
            .setLevel(vk::CommandBufferLevel::ePrimary)
        );
    }

    void RecordCommands(const std::vector<vk::UniqueCommandBuffer>& cmdBuffers, const std::vector<vk::UniqueFramebuffer>& framebuffer, const vk::RenderPass& renderPass, Pipeline& pipeline, const vk::Extent2D& extents) {

        int index = -1;
        // Record commands for each framebuffer and pick the right one at runtime
        for (const auto& cmdBuffer : cmdBuffers) {
            ++index;
            const auto commandBufferBeginInfo { vk::CommandBufferBeginInfo() };

            const auto& clearValues{ vk::ClearValue()
                .setColor(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}))
            };

            const auto& renderPassBeginInfo{ vk::RenderPassBeginInfo()
                .setRenderPass(renderPass)
                .setClearValueCount(1)
                .setPClearValues(&clearValues)
                .setFramebuffer(framebuffer[index].get())
                .setRenderArea(vk::Rect2D()
                    .setExtent(extents)
                    .setOffset({0, 0})
                )
            };

            cmdBuffer.get().begin(commandBufferBeginInfo);
            cmdBuffer.get().beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
            cmdBuffer.get().bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetPipeline());
            cmdBuffer.get().draw(3, 1, 0, 0);
            cmdBuffer.get().endRenderPass();
            cmdBuffer.get().end();
        }
    }
}
