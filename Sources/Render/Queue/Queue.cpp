#include "Queue.hpp"
#include "Logger.hpp"
#include <iostream>

namespace Engine::Render::Queue {

    QueueManager::QueueManager(const vk::PhysicalDevice& phyDev, const vk::SurfaceKHR& surface) {

        const auto queueFams{ phyDev.getQueueFamilyProperties2() };

        const auto vkQFB_Graphics   { VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT        };
        const auto vkQFB_Compute    { VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT         };
        const auto vkQFB_Transfer   { VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT        };
        const auto vkQFB_Sparse     { VkQueueFlagBits::VK_QUEUE_SPARSE_BINDING_BIT  };
        const auto vkQFB_Protec     { VkQueueFlagBits::VK_QUEUE_PROTECTED_BIT       };

        int queueInx{ -1 };
        for (auto& queueFam : queueFams) {
            queueInx++;

            const auto  queueProperties{ queueFam.queueFamilyProperties };
            const auto  flags{ queueProperties.queueFlags };
            QueueType   queueFamilyType{};

            QueueFamily qf{};
            qf.Count    = queueProperties.queueCount;
            qf.Flags    = queueProperties.queueFlags;
            qf.Index    = queueInx;
            qf.Used     = 0;
            qf.PresentSupport = phyDev.getSurfaceSupportKHR(queueInx, surface) == VK_TRUE;

            // Now, we evaluate QueueType so it's easier to get
            // stuff later on.
            // transfer only (eCompute and eTransfer must not be present)
            //switch (vk::QueueFlags)

            // If there are two families that evaluate to the same QueueType,
            // then this code will crash and burn. I don't see devices where
            // that can happen though
            switch (static_cast<VkQueueFlags>(flags)) {

                case vkQFB_Graphics | vkQFB_Compute:
                case vkQFB_Graphics | vkQFB_Compute | vkQFB_Transfer:
                case vkQFB_Graphics | vkQFB_Compute | vkQFB_Transfer | vkQFB_Sparse:
                case vkQFB_Graphics | vkQFB_Compute | vkQFB_Transfer | vkQFB_Sparse | vkQFB_Protec:
                    queueFamilyType = QueueType::Graphics;
                    break;

                case vkQFB_Compute | vkQFB_Transfer:
                case vkQFB_Compute | vkQFB_Transfer | vkQFB_Sparse:
                case vkQFB_Compute | vkQFB_Transfer | vkQFB_Sparse | vkQFB_Protec:
                    queueFamilyType = QueueType::Compute;
                    break;

                case vkQFB_Transfer:
                case vkQFB_Transfer | vkQFB_Sparse:
                case vkQFB_Transfer | vkQFB_Sparse | vkQFB_Protec:
                    queueFamilyType = QueueType::Transfer;
                    break;

                default:
                    throw std::runtime_error("Unexpected queue case.");
            }

            if (queueFamilies.count(queueFamilyType) > 0)
                throw std::runtime_error("Same queue type found.");

            queueFamilies.emplace(queueFamilyType, qf);

            LOGGER << "\t Queue Familiy [" << queueInx << "]: \n";
            LOGGER << "\t\t Queue Count: "                          << queueFamilies.at(queueFamilyType).Count          << '\n';
            LOGGER << "\t\t Present Support: "  << std::boolalpha   << queueFamilies.at(queueFamilyType).PresentSupport << '\n';
            LOGGER << "\t\t Queue flags: "      << vk::to_string(queueFam.queueFamilyProperties.queueFlags)             << '\n';
        }
    }

    // Reserve queues before logical device creation.
    // allocateQueues <QueueType to allocate, how many of the type to allocate>
    QueueManager::QueueManager(const vk::PhysicalDevice& phyDev, const vk::SurfaceKHR& surface, const std::map<QueueType, int>& allocateQueues) :
        QueueManager::QueueManager(phyDev, surface) {

        for (const auto& i : allocateQueues) {
            ReserveQueues(i.first, i.second);
        }
    }


    void QueueManager::ReserveQueues(const QueueType queueType, const int howMany) {
        assert(queueFamilies.at(queueType).QueuesAvailable(howMany) && "No Queues available");
        queueFamilies.at(queueType).Used += howMany;
    }

    void QueueManager::PopulateQueues(const vk::Device& renderDevice) {
        for (const auto& queueFam : queueFamilies) {
            if (queueFam.second.Used > 0) {
                for (uint32_t i = 0; i < queueFam.second.Used; ++i) {
                    allocatedQueues.emplace(queueFam.first, renderDevice.getQueue(queueFam.second.Index, i));
                }
            }
        }
    }

    const std::map<QueueType, QueueFamily>& QueueManager::RequestedQueues() const {
        return queueFamilies;
    }

    // Returns the queue family of specified type
    const QueueFamily& QueueManager::GetQF(const QueueType qt) const {
        return queueFamilies.at(qt);
    }

    // Returns the allocated queue.
    const vk::Queue& QueueManager::GetQ(const QueueType qt) const {
        return allocatedQueues.at(qt);
    }

    // Returns the allocated queue.
    vk::Queue& QueueManager::operator[] (const QueueType queueType) {
        return allocatedQueues.at(queueType);
    }

}
