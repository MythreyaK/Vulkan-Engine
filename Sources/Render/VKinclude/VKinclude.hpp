#ifndef VKINCLUDE_HPP

#define VKINCLUDE_HPP
// This file ensures all vulkan.hpp includes in the
// project use these preprosessor directives

#define VK_USE_PLATFORM_WIN32_KHR
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#define LOG std::cerr

#include <vulkan/vulkan.hpp>

static vk::DynamicLoader dynamicLoader;

#endif // !VKINCLUDE_HPP
