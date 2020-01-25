#ifndef RENDER_QUEUE_HPP
#define RENDER_QUEUE_HPP

#include "VKinclude/VKinclude.hpp"
#include <map>

namespace Engine::Render::Queue {

    // vk::QueueFlagBits = vkQB
    enum class QueueType : char {
        Graphics,      // vkQB::eGraphics + vkQB::eCompute (implies vkQB::eTransfer)
        Compute,       // vkQB::eCompute (implies vkQB::eTransfer)
        Transfer,      // vkQB::eTransfer, (any of vkQB::eSparseBinding, vkQB::eProtected may be present)
        All = Graphics // vkQB::eGraphics + vkQB::eCompute + vkQB::eTransfer
    };
    // If a eGraphics only queue exists, there must also exist a queue with
    // support for eGraphics + eCompute. Each of the flags also imply transfer
    // support. Hence, a Graphics queue is also a general-purpose queue
    // Well, I hope my understanding of the spec is correct, we'll see.

    struct QueueFamily {
        uint32_t Index   { 0 };
        uint32_t Count   { 0 };
        uint32_t Used    { 0 };
        bool PresentSupport{ false };
        vk::QueueFlags Flags;

        const bool QueuesAvailable()                    { return Used < Count; }
        const bool QueuesAvailable(const uint32_t newAlloc)  { return Used + newAlloc < Count; }
    };


    class QueueManager {
    private:

        std::map<QueueType, QueueFamily>    queueFamilies;
        std::map<QueueType, vk::Queue>      allocatedQueues;

    public:
        QueueManager(const vk::PhysicalDevice&, const vk::SurfaceKHR&);
        QueueManager(const vk::PhysicalDevice&, const vk::SurfaceKHR&, const std::map<QueueType, int>&);
        QueueManager(QueueManager&&) = default;
        QueueManager& operator=(QueueManager&&) = default;

        // Disable copies
        QueueManager(const QueueManager&) = delete;
        QueueManager& operator=(const QueueManager&) = delete;

        void    ReserveQueues(const QueueType, const int);
        void    PopulateQueues(const vk::Device&);
        const std::map<QueueType, QueueFamily>& RequestedQueues() const;
        const QueueFamily&  GetQF(const QueueType qt) const;
        const vk::Queue&    GetQ (const QueueType qt) const;

        // NOTE: Queue must have been allocated
        vk::Queue&   operator[](const QueueType);

    };
}

#endif
