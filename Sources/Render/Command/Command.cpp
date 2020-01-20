#include "Command.hpp"
#include "Pipeline\Pipeline.hpp"



namespace Engine::Render::Command {

    namespace ERQU = Engine::Render::Queue;

    void RecordGraphicsCommandBuffers(std::vector<vk::UniqueCommandBuffer>& cmdBuffers, const std::vector<vk::UniqueFramebuffer>& framebuffer, const vk::RenderPass& renderPass, Pipeline& pipeline, const vk::Extent2D& extents);

    std::map<ERQU::QueueType, vk::UniqueCommandPool> CreateQueueCommandPool(const vk::Device& renderDevice, const ERQU::QueueManager& qmg) {
        std::map<Engine::Render::Queue::QueueType, vk::UniqueCommandPool> cmdPools{};

        for (const auto& i : qmg.RequestedQueues()) {
            if (i.second.Used > 0) {
                cmdPools.emplace(i.first, renderDevice.createCommandPoolUnique(vk::CommandPoolCreateInfo()
                    .setQueueFamilyIndex(i.second.Index))
                );
            }
        }
        return cmdPools;
    }

    std::map<ERQU::QueueType, std::vector<vk::UniqueCommandBuffer>> CreateCommandBuffers(const vk::Device& renderDevice, const std::map<ERQU::QueueType, vk::UniqueCommandPool>& cmdPools, const uint32_t numBuffers) {
        std::map<ERQU::QueueType, std::vector<vk::UniqueCommandBuffer>> ret{};

        for (const auto& cmdPool : cmdPools) {
            ret.emplace(cmdPool.first, renderDevice.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo()
                .setCommandPool(cmdPool.second.get())
                .setCommandBufferCount(numBuffers)
                .setLevel(vk::CommandBufferLevel::ePrimary)
            ));
        }

        return ret;
    }

    void RecordCommands(const ERQU::QueueType qt, std::vector<vk::UniqueCommandBuffer>& cmdBuffers, const std::vector<vk::UniqueFramebuffer>& framebuffer, const vk::RenderPass& renderPass, Pipeline& pipeline, const vk::Extent2D& extents) {
        // TODO: Maybe use std::functional?
 
        switch (qt) {
            case ERQU::QueueType::Graphics:
                RecordGraphicsCommandBuffers(cmdBuffers, framebuffer, renderPass, pipeline, extents);
                break;
            default:
                throw std::runtime_error("This command buffer type is not handled");
        }

    }

    void RecordGraphicsCommandBuffers(std::vector<vk::UniqueCommandBuffer>& cmdBuffers, const std::vector<vk::UniqueFramebuffer>& framebuffer, const vk::RenderPass& renderPass, Pipeline& pipeline, const vk::Extent2D& extents) {

        int index{ -1 };
        // Record commands for each framebuffer and pick the right one at runtime
        for (const auto& cmdBuffer : cmdBuffers) {
            ++index;
            const auto commandBufferBeginInfo { vk::CommandBufferBeginInfo() };

            const auto clearValues{ vk::ClearValue()
                .setColor(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}))
            };

            const auto renderPassBeginInfo{ vk::RenderPassBeginInfo()
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
