#include "Surface.hpp"

#include <iostream>

extern "C" {
    VkResult glfwCreateWindowSurface(   VkInstance,
                                        GLFWwindow*,
                                        const VkAllocationCallbacks*,
                                        VkSurfaceKHR*);
}

namespace Engine::Render::Surface {

    vk::UniqueSurfaceKHR CreateSurface(vk::Instance& inst, WindowHandle* handle) {
        VkSurfaceKHR my_surface;

        if (VK_SUCCESS == glfwCreateWindowSurface(inst.operator VkInstance(), handle, nullptr, &my_surface)) {

            vk::ObjectDestroy<vk::Instance, vk::DispatchLoaderDynamic> surfaceDeleter(inst, nullptr, VULKAN_HPP_DEFAULT_DISPATCHER);

            LOG << "Surface Creation complete\n";

            return vk::UniqueSurfaceKHR(vk::SurfaceKHR(my_surface), surfaceDeleter);
        }
        else throw std::runtime_error("Could not create Vulkan surface.");
    }
}
