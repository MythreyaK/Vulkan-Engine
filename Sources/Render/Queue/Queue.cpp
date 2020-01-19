#include "Queue.hpp"
#include "Logger.hpp"
#include <iostream>

namespace Engine::Render::Queue {

    const std::string StringifyQueueType(const vk::QueueFlags& qf);

    QueueManager::QueueManager(const vk::PhysicalDevice& phyDev, const vk::SurfaceKHR& surface) {
        // If there are two families that evaluate to the same QueueType,
        // then this code will crash and burn. I don't see devices where
        // that can happen though.
        const auto queueFams{ phyDev.getQueueFamilyProperties2() };

        using vkQFB = vk::QueueFlagBits;

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
            if ( (flags & vkQFB::eTransfer) && !(flags & vkQFB::eCompute & vkQFB::eGraphics))
                queueFamilyType = QueueType::Transfer;

            // Compute only, no eGraphics
            else if ( (flags & vkQFB::eCompute) && !(flags & vkQFB::eGraphics))
                queueFamilyType = QueueType::Compute;

            else queueFamilyType = QueueType::All;

            // look at the note above for why this check exists
            if (queueFamilies.count(queueFamilyType) > 0)
                throw std::runtime_error("Runtime check failed");

            queueFamilies.emplace(queueFamilyType, qf);

            LOGGER << "\t Queue Familiy [" << queueInx << "]: \n";
            LOGGER << "\t\t Queue Count: "                          << queueFamilies.at(queueFamilyType).Count          << '\n';
            LOGGER << "\t\t Present Support: "  << std::boolalpha   << queueFamilies.at(queueFamilyType).PresentSupport << '\n';
            LOGGER << "\t\t Queue flags: "      << StringifyQueueType(queueFam.queueFamilyProperties.queueFlags)        << '\n';
        }
    }

    void QueueManager::ReserveQueue(const QueueType queueType) {
        assert(queueFamilies.at(queueType).QueuesAvailable() && "No Queues available");
        queueFamilies.at(queueType).Used++;
    }

    void QueueManager::PopulateQueues(const vk::Device& renderDevice) {
        for (const auto& queueFam : queueFamilies) {
            for (int i = 0; i < queueFam.second.Used; ++i) {
                allocatedQueues.emplace(queueFam.first, renderDevice.getQueue(queueFam.second.Index, i));
            }
        }
    }

    const std::map<QueueType, QueueFamily> QueueManager::GetRequestedQueues() const {
        return queueFamilies;
    }

    vk::Queue& QueueManager::operator[] (const QueueType queueType) {
        return allocatedQueues.at(queueType);
    }


    const std::string StringifyQueueType(const vk::QueueFlags& qf) {
        std::string stringbuff{ "[ " };

        if (qf & vk::QueueFlagBits::eCompute)       stringbuff += "Compute, ";
        if (qf & vk::QueueFlagBits::eTransfer)      stringbuff += "Transfer, ";
        if (qf & vk::QueueFlagBits::eGraphics)      stringbuff += "Graphics, ";
        if (qf & vk::QueueFlagBits::eProtected)     stringbuff += "Protected, ";
        if (qf & vk::QueueFlagBits::eSparseBinding) stringbuff += "SparseBinding, ";

        // remove the comma and space, add a space
        return stringbuff += "\b\b ]";
    }

}
