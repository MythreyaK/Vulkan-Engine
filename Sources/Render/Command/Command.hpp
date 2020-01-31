#ifndef RENDER_COMMAND_HPP
#define RENDER_COMMAND_HPP

#include "VKinclude/VKinclude.hpp"
#include "Queue/Queue.hpp"
#include "Memory/Buffers.hpp"

namespace Engine::Render {
    class Pipeline;
}

namespace Engine::Render::Command {

    namespace ERQU = Engine::Render::Queue;

    std::map<ERQU::QueueType, vk::UniqueCommandPool> CreateQueueCommandPool (const vk::Device& phyDev, const ERQU::QueueManager& qmg);
    std::map<ERQU::QueueType, std::vector<vk::UniqueCommandBuffer>> CreateCommandBuffers(const vk::Device& renderDevice, const std::map<ERQU::QueueType, vk::UniqueCommandPool>& cmdPools, const uint32_t numBuffers);

    template <typename T>
    void RecordGraphicsCommandBuffers(std::vector<vk::UniqueCommandBuffer>& cmdBuffers, const std::vector<vk::UniqueFramebuffer>& framebuffer, const vk::RenderPass& renderPass, Pipeline& pipeline, const vk::Extent2D& extents, const Engine::Render::Memory::DeviceMemory<T>& buffer);
    void RecordCommands(const ERQU::QueueType, std::vector<vk::UniqueCommandBuffer>& cmdBuffers, const std::vector<vk::UniqueFramebuffer>& framebuffer, const vk::RenderPass& renderPass, Engine::Render::Pipeline& pipeline, const vk::Extent2D& extents);

    template <typename T>
    void RecordGraphicsCommandBuffers(std::vector<vk::UniqueCommandBuffer>& cmdBuffers, const std::vector<vk::UniqueFramebuffer>& framebuffer, const vk::RenderPass& renderPass, Pipeline& pipeline, const vk::Extent2D& extents, const Engine::Render::Memory::DeviceMemory<T>& v) {

        int index{ -1 };
        // Record commands for each framebuffer and pick the right one at runtime
        for (const auto& cmdBuffer : cmdBuffers) {
            ++index;
            const auto commandBufferBeginInfo{ vk::CommandBufferBeginInfo() };

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
            vk::DeviceSize offsets{};
            cmdBuffer.get().bindVertexBuffers(0, 1, v.Buffer(), &offsets);
            cmdBuffer.get().draw(v.Size(), 1, 0, 0);
            cmdBuffer.get().endRenderPass();
            cmdBuffer.get().end();
        }
    }

}



#endif // !RENDER_COMMAND_HPP
