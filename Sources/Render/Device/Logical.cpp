#include "Logical.hpp"

#include "DeviceExtensions.hpp"
#include "Device/Physical.hpp"

#include <set>

namespace Engine::Render::Device::Logical {

    vk::UniqueDevice CreateLogicalDevice(const PhysicalDevice& devInfo, vk::SurfaceKHR& surface) {

        const float priority{ 1.0 };

        std::vector<vk::DeviceQueueCreateInfo> queuesCreateInfos{};
        const std::set<uint32_t> uniqueQueueFamilies{ devInfo.GraphicsQueue(), devInfo.PresentQueue() };

        for (auto& i : uniqueQueueFamilies) {
            const auto qCreateInfo{ vk::DeviceQueueCreateInfo()
                .setQueueFamilyIndex(i)
                .setQueueCount(1)
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
        auto renderDevice{ devInfo.Get().createDeviceUnique(logicalDeviceCreateInfo) };
        VULKAN_HPP_DEFAULT_DISPATCHER.init(renderDevice.get());

        return renderDevice;
    }


    std::vector<vk::Queue> GetQueues(const vk::Device& renderDevice, const PhysicalDevice& devInfo) {
        // TODO: Make this a dictionary, so it's easier to retrieve handles
        std::vector<vk::Queue> queues{};

        const std::set<uint32_t> uniqueQueueFamilies{ devInfo.GraphicsQueue(), devInfo.PresentQueue() };

        for (uint32_t i : uniqueQueueFamilies) {
            queues.emplace_back(
                renderDevice.getQueue2(vk::DeviceQueueInfo2()
                    .setQueueIndex(0)
                    .setQueueFamilyIndex(i)
                )
            );
        }

        if (queues.size() > 1)
            assert(0 == "TODO: Handle case when queues are not the same");

        return queues;
    }

    // TODO: Allow for creation of n number of queues? Usually not required,
    // since queue info must be specified before creation of logical device.
}
