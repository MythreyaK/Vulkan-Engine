#ifndef RENDER_DEBUG_UTILS_HPP

#define RENDER_DEBUG_UTILS_HPP

#include "VKinclude/VKinclude.hpp"

namespace Engine::Render::Debug {

    using CallbackFuncion = PFN_vkDebugUtilsMessengerCallbackEXT;

    using MSevr = vk::DebugUtilsMessageSeverityFlagBitsEXT;
    using MType = vk::DebugUtilsMessageTypeFlagBitsEXT;
    using DebugMessenger = vk::UniqueDebugUtilsMessengerEXT;

    DebugMessenger CreateDebugMessenger(vk::Instance&, CallbackFuncion funcPointer, void* userData);
}

#endif // !RENDER_DEBUG_UTILS_HPP
