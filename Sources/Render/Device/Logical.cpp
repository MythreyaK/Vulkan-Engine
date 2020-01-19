#include "Logical.hpp"

#include "DeviceExtensions.hpp"
#include "Device/Physical.hpp"
#include "Queue/Queue.hpp"

#include <set>

namespace Engine::Render::Device::Logical {

    vk::UniqueDevice CreateLogicalDevice(PhysicalDevice& phyDev, vk::SurfaceKHR& surface) {

        const float priority{ 1.0 };

        std::vector<vk::DeviceQueueCreateInfo> queuesCreateInfos{};

        for (const auto& queueFam : phyDev.QueueManager().GetRequestedQueues()) {
            const auto qCreateInfo{ vk::DeviceQueueCreateInfo()
                .setQueueFamilyIndex(queueFam.second.Index)
                .setQueueCount(queueFam.second.Used)
                .setPQueuePriorities(&priority)
            };
            queuesCreateInfos.emplace_back(qCreateInfo);
        }

        const auto logicalDeviceCreateInfo{ vk::DeviceCreateInfo()
            // TODO: Hmmmm featuressss....
            .setPEnabledFeatures({})
            .setPQueueCreateInfos(queuesCreateInfos.data())
            .setQueueCreateInfoCount(static_cast<uint32_t>(queuesCreateInfos.size()))
            .setEnabledExtensionCount(static_cast<uint32_t>(requiredDeviceExtensions.size()))
            .setPpEnabledExtensionNames(requiredDeviceExtensions.data())
        };

        // Create logical device and get device speficic pointers
        auto renderDevice{ phyDev.Get().createDeviceUnique(logicalDeviceCreateInfo) };
        VULKAN_HPP_DEFAULT_DISPATCHER.init(renderDevice.get());

        phyDev.QueueManager().PopulateQueues(renderDevice.get());
        return renderDevice;
    }
}
