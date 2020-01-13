#ifndef RENDER_INSTANCE_HPP

#define RENDER_INSTANCE_HPP

#include "VKinclude/VKinclude.hpp"
#include <vector>
#include <optional>


struct GLFWwindow;
typedef GLFWwindow WindowHandle;


namespace Engine::Render::Instance {

    using ch_vec = std::vector<const char*>;

    vk::UniqueInstance CreateInstance(
        const ch_vec& requiredExtensions,
        const std::optional<ch_vec> validationLayers,
        WindowHandle* glfwWindowHandle);


    const bool CheckValidationLayerSupport(const std::vector<const char*>&);
    ch_vec& GetValidationLayers(std::optional<ch_vec> requestedLayers);
}


#endif // !INSTANCE_HPP
