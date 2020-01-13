#ifndef RENDER_SURFACE_HPP

#define RENDER_SURFACE_HPP

#include "VKinclude/VKinclude.hpp"

extern "C" {
    struct GLFWwindow;
}

typedef GLFWwindow WindowHandle;

namespace Engine::Render::Surface {

    vk::UniqueSurfaceKHR CreateSurface(vk::Instance&, WindowHandle*);

}

#endif // !RENDER_SURFACE_HPP
