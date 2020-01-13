#ifndef RENDER_PIPELINE_SHADER_HPP
#define RENDER_PIPELINE_SHADER_HPP

#include "VKinclude/VKinclude.hpp"

#include <vector>
#include <string>

namespace Engine::Render::Shader {

    vk::UniqueShaderModule CreateShaderModule(const vk::Device&, const std::string& fname);
}


#endif // !RENDER_PIPELINE_SHADER_HPP
