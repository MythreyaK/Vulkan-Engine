#ifndef RENDER_LOGICAL_DEVICE_HPP

#define RENDER_LOGICAL_DEVICE_HPP

#include "VKinclude/VKinclude.hpp"

namespace Engine::Render::Device {
    class PhysicalDevice;
}

namespace Engine::Render::Device::Logical {

    using ch_vec = std::vector<const char*>;

    vk::UniqueDevice CreateLogicalDevice(PhysicalDevice&, vk::SurfaceKHR&);

}

#endif // !RENDER_LOGICAL_DEVICE_HPP
