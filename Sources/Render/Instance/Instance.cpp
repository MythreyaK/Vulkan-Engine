#include "Instance.hpp"
#include "Logger.hpp"
#include "Version.hpp"

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
        WindowHandle* handle) {

        // Initialize the dynamic loader
        VULKAN_HPP_DEFAULT_DISPATCHER.init(
            dynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr")
        );

        // Debug/Validation layers
#       ifdef BUILD_TYPE_DEBUG
        ch_vec requiredExtnsWithDebug{ requiredExtns };
        requiredExtnsWithDebug.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        const ch_vec& val_layers{ GetValidationLayers(validationLayers) };

        if (!CheckValidationLayerSupport(val_layers))
            throw std::runtime_error("Requested Validation layers not available.");
#       endif // BUILD_TYPE_DEBUG


        // App info 
        const auto appInfo{ vk::ApplicationInfo()
            .setPApplicationName("Vulkan App")
            .setApiVersion(VK_API_VERSION_1_1)
            .setApplicationVersion(1)
            .setEngineVersion(1)
            .setPEngineName("Vulkan Engine")
        };


        // Instance create info
        const auto instCreateInfo{ vk::InstanceCreateInfo()
            .setFlags(vk::InstanceCreateFlags())
            .setPApplicationInfo(&appInfo)
#           ifdef BUILD_TYPE_DEBUG
            // use required + debug extension for debug
            .setEnabledLayerCount(static_cast<uint32_t>(val_layers.size()))
            .setPpEnabledLayerNames(val_layers.data())
            .setEnabledExtensionCount(static_cast<uint32_t>(requiredExtnsWithDebug.size()))
            .setPpEnabledExtensionNames(requiredExtnsWithDebug.data())
        };
#           else
            // else just use the required extensions
            .setEnabledLayerCount(0)
            .setPpEnabledLayerNames(nullptr)
            .setEnabledExtensionCount(static_cast<uint32_t>(requiredExtns.size()))
            .setPpEnabledExtensionNames(requiredExtns.data())
        };
#       endif // BUILD_TYPE_DEBUG

        // Actually cerate the instance
        auto vulkanInstance{ vk::createInstanceUnique(instCreateInfo) };

        VULKAN_HPP_DEFAULT_DISPATCHER.init(vulkanInstance.get());

        return vulkanInstance;
    }


    const bool CheckValidationLayerSupport(const ch_vec& requestedLayers) {

        const auto supportedLayers{ vk::enumerateInstanceLayerProperties() };

        int nLayersFound{ 0 };

        for (const auto& supp_layer : supportedLayers) {

            bool foundThis{ false };

            for (const auto& req_lr : requestedLayers) {
                if (strncmp(req_lr, supp_layer.layerName, 255) == 0) {
                    foundThis = true;
                    nLayersFound++;
                }
            }

            if (foundThis) {
                // layer found, this can be enabled
                LOGGER << "[E] " << supp_layer.layerName << '\n';
            }
            else {
                LOGGER << "[D] " << supp_layer.layerName << '\n';
            }
        }

        return nLayersFound == requestedLayers.size();
    }


    const ch_vec& GetValidationLayers(const std::optional<ch_vec>& requestedLayers) {

        if (requestedLayers.has_value()) {
            return requestedLayers.value();
        }
        else {
            return defaultValidationLayers;
        }
    }

}
