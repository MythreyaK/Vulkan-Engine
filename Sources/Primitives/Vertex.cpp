#include "Vertex.hpp"

namespace Engine::Primitives {

    const uint64_t Vertex::Size(uint64_t numberOfVertices) {
        return sizeof(Vertex) * numberOfVertices;
    }

    const vk::VertexInputBindingDescription* Vertex::Binding() {
        static const vk::VertexInputBindingDescription vb{
            0,                  // Binding
            sizeof(Vertex),     // Stride
            vk::VertexInputRate::eVertex
        };

        return &vb;
    }

    const std::array<const vk::VertexInputAttributeDescription, 2>& Vertex::Attributes() {

        static const std::array<const vk::VertexInputAttributeDescription, 2> vat{
            // For position
            vk::VertexInputAttributeDescription{
                0,                          // location
                0,                          // binding
                vk::Format::eR32G32Sfloat,  // format
                offsetof(Vertex, pos)       // offset
            },
            // For color
            vk::VertexInputAttributeDescription{
                1,                            // location
                0,                            // binding
                vk::Format::eR32G32B32Sfloat, // format
                offsetof(Vertex, col)         // offset
            }
        };

        return vat;
    }

}
