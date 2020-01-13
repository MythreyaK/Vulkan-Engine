#include "Instance.hpp"
#include <iostream>

namespace Engine::Render::Instance {

    std::vector<const char*> defaultValidationLayers {
        "VK_LAYER_LUNARG_standard_validation",
        "VK_LAYER_KHRONOS_validation"
    };

    // All functions

    vk::UniqueInstance CreateInstance(
        const std::vector<const char*>& requiredExtns,
        const std::optional<std::vector<const char*>> validationLayers,
        WindowHandle* handle)
    {

        // Initialize the dynamic loader
        VULKAN_HPP_DEFAULT_DISPATCHER.init(
            dynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>(
                "vkGetInstanceProcAddr")
        );

        // Debug/Validation layers
#       ifndef NDEBUG

        ch_vec enabled_extns{ requiredExtns };
        enabled_extns.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        const ch_vec& val_layers = GetValidationLayers(validationLayers);

        if (!CheckValidationLayerSupport(val_layers))
            throw std::runtime_error("Requested Validation layers not available.");
#       endif // !NDEBUG


        // App info 
        const auto& appInfo = vk::ApplicationInfo()
            .setPApplicationName("Vulkan App")
            .setApiVersion(VK_API_VERSION_1_1)
            .setApplicationVersion(1)
            .setEngineVersion(1)
            .setPEngineName("Vulkan Engine");


        // Instance create info
        const auto& instCreateInfo = vk::InstanceCreateInfo()
            .setFlags(vk::InstanceCreateFlags())
            .setPApplicationInfo(&appInfo)
        // use required + debug extension for debug
#       ifndef NDEBUG
            .setEnabledLayerCount(static_cast<uint32_t>(val_layers.size()))
            .setPpEnabledLayerNames(val_layers.data())
            .setEnabledExtensionCount(static_cast<uint32_t>(enabled_extns.size()))
            .setPpEnabledExtensionNames(enabled_extns.data());
        // else just use the required extensions
#       else
            .setPpEnabledExtensionNames(requiredExtns.data())
            .setEnabledExtensionCount(static_cast<uint32_t>(requiredExtns.size()))
            .setEnabledLayerCount(0)
            .setPpEnabledLayerNames(nullptr);
#       endif // !NDEBUG

        // Actually cerate the instance
        auto vulkanInstance = vk::createInstanceUnique(instCreateInfo);

        VULKAN_HPP_DEFAULT_DISPATCHER.init(vulkanInstance.get());

        return vulkanInstance;
    }


    const bool CheckValidationLayerSupport(const ch_vec& requestedLayers) {

        const auto& supportedLayers = vk::enumerateInstanceLayerProperties();

        int nLayersFound{ 0 };

        for (const auto& supp_layer : supportedLayers) {

            bool foundThis{ false };

            for (const auto& req_lr : requestedLayers) {
                if (strncmp(req_lr, supp_layer.layerName, 255) == 0) {
                    foundThis = true;
                    nLayersFound++;
                }
            }

#           ifndef NDEBUG
            if (foundThis) {
                // layer found, this can be enabled
                LOG << "[E] " << supp_layer.layerName << '\n';
            }
            else {
                LOG << "[D] " << supp_layer.layerName << '\n';
            }
#           endif // !NDEBUG
        }

        return nLayersFound == requestedLayers.size();
    }


    ch_vec& GetValidationLayers(std::optional<ch_vec> requestedLayers) {

        if (requestedLayers.has_value()) {
            return requestedLayers.value();
        }
        else {
            return defaultValidationLayers;
        }
    }

}
