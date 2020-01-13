#ifndef RENDER_RENDERPASS_HPP
#define RENDER_RENDERPASS_HPP

#include "VKinclude/VKinclude.hpp"

namespace Engine::Render::Device {
    class PhysicalDevice;
}

namespace Engine::Render::RenderPass {

    namespace ERD = Engine::Render::Device;

    vk::UniqueRenderPass CreateRenderPass(const vk::Device& device, const ERD::PhysicalDevice& devInfo);

}


#endif // !RENDER_RENDERPASS_HPP
