#ifndef RENDER_COMMAND_HPP
#define RENDER_COMMAND_HPP

#include "VKinclude/VKinclude.hpp"
#include "Queue/Queue.hpp"

namespace Engine::Render {
    class Pipeline;
}

namespace Engine::Render::Command {

    namespace ERQU = Engine::Render::Queue;

    std::map<ERQU::QueueType, vk::UniqueCommandPool> CreateQueueCommandPool (const vk::Device& phyDev, const ERQU::QueueManager& qmg);
    std::map<ERQU::QueueType, std::vector<vk::UniqueCommandBuffer>> CreateCommandBuffers(const vk::Device& renderDevice, const std::map<ERQU::QueueType, vk::UniqueCommandPool>& cmdPools, const uint32_t numBuffers);

    void RecordCommands(const ERQU::QueueType, std::vector<vk::UniqueCommandBuffer>& cmdBuffers, const std::vector<vk::UniqueFramebuffer>& framebuffer, const vk::RenderPass& renderPass, Engine::Render::Pipeline& pipeline, const vk::Extent2D& extents);
}

#endif // !RENDER_COMMAND_HPP
