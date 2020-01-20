#ifndef RENDER_LOGICAL_DEVICE_HPP
#define RENDER_LOGICAL_DEVICE_HPP

#include "VKinclude/VKinclude.hpp"

namespace Engine::Render::Device {
    class PhysicalDevice;
}

namespace Engine::Render::Queue {
    enum class QueueType : char;
    struct QueueFamily;
    class QueueManager;
}

namespace Engine::Render::Device::Logical {

    vk::UniqueDevice CreateLogicalDevice(PhysicalDevice&, vk::SurfaceKHR&, Engine::Render::Queue::QueueManager&);
}

#endif // !RENDER_LOGICAL_DEVICE_HPP
