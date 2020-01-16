#include "Shader.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>

namespace Engine::Render::Shader {

    std::vector<std::byte> ReadShader(const std::string& fname);


    vk::UniqueShaderModule CreateShaderModule(const vk::Device& device, const std::string& fname) {
        const auto buffer{ ReadShader(fname) };
        const auto shaderModuleCreateInfo{ vk::ShaderModuleCreateInfo()
            .setCodeSize(buffer.size())
            .setPCode(reinterpret_cast<const uint32_t*>(buffer.data()))
        };

        return device.createShaderModuleUnique(shaderModuleCreateInfo);
    }


    std::vector<std::byte> ReadShader(const std::string& fname) {

        namespace fs = std::filesystem;

        std::ifstream file(fname, std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        std::vector<std::byte> shader_buffer(fs::file_size(fname));

        file.read(reinterpret_cast<char*>(shader_buffer.data()), shader_buffer.size());
        file.close();

        return shader_buffer;
    }

}
