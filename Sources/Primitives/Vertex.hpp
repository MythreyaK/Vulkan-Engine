#ifndef ENGINE_MESH_PRIMITIVES_HPP
#define ENGINE_MESH_PRIMITIVES_HPP

#include "Render/VKinclude/VKinclude.hpp"

#include <glm/glm.hpp>
#include <array>

namespace Engine::Primitives {

    struct Vertex {
        glm::vec2 pos;
        glm::vec3 col;

        static const vk::VertexInputBindingDescription* Binding();
        static const uint64_t Size(uint64_t vertices);
        static const std::array<const vk::VertexInputAttributeDescription, 2>& Attributes();
    };

}


#endif
