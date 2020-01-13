#include "VKinclude/VKinclude.hpp"
#include <vector>

#ifndef RENDER_REQUIRED_DEVICE_EXTENSIONS
#define RENDER_REQUIRED_DEVICE_EXTENSIONS

namespace Engine::Render::Device {

    const std::vector<const char*> requiredDeviceExtensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

}

#endif // !RENDER_REQUIRED_DEVICE_EXTENSIONS

