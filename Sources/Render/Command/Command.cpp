#include "Command.hpp"
#include "Pipeline\Pipeline.hpp"


namespace Engine::Render::Command {

    namespace ERQU = Engine::Render::Queue;

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
                // RecordGraphicsCommandBuffers(cmdBuffers, framebuffer, renderPass, pipeline, extents);
                break;
            default:
                throw std::runtime_error("This command buffer type is not handled");
        }

    }

    


}
