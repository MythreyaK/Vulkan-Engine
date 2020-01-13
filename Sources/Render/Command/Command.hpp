#ifndef RENDER_COMMAND_HPP
#define RENDER_COMMAND_HPP

#include "VKinclude/VKinclude.hpp"

namespace Engine::Render {
    class Pipeline;
}

namespace Engine::Render::Command {

    vk::UniqueCommandPool CreateQueueCommandPool (const vk::Device& phyDev, const uint32_t graphicsQueueFamily);
    std::vector<vk::UniqueCommandBuffer> CreateCommandBuffer(const vk::Device& renderDevice, const vk::CommandPool& cmdPool, const uint32_t numBuffers);

    void RecordCommands(const std::vector<vk::UniqueCommandBuffer>& cmdBuffers, const std::vector<vk::UniqueFramebuffer>& framebuffer, const vk::RenderPass& renderPass, Engine::Render::Pipeline& pipeline, const vk::Extent2D& extents);
}

#endif // !RENDER_COMMAND_HPP
