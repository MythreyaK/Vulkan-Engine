#ifndef RENDER_QUEUE_HPP
#define RENDER_QUEUE_HPP

#include "VKinclude/VKinclude.hpp"
#include <map>

namespace Engine::Render::Queue {

    enum class QueueType : char {
        // vk::QueueFlagBits = vkQB
        Compute,        // vkQB::eCompute                   (implies vkQB::eTransfer)
        Transfer,       // vkQB::eTransfer, (any of vkQB::eSparseBinding, vkQB::eProtected may be present)
        Graphics,       // vkQB::eGraphics + vkQB::eCompute (implies vkQB::eTransfer)
        All = Graphics  // vkQB::eGraphics + vkQB::eCompute + vkQB::eTransfer
        // If a eGraphics only queue exists, there must also exist a queue with
        // support for eGraphics + eCompute. Each of the flags also imply transfer
        // support. Hence, a Graphics queue is also a general-purpose queue
        // Well, I hope my understanding of the spec is correct, we'll see.
    };

    struct QueueFamily {
        int Index   { 0 };
        int Count   { 0 };
        int Used    { 0 };
        bool PresentSupport{ false };
        vk::QueueFlags Flags;

        const bool QueuesAvailable() { return Used < Count; }
    };


    class QueueManager {
    private:

        std::map<QueueType, QueueFamily>    queueFamilies;
        std::map<QueueType, vk::Queue>      allocatedQueues;

    public:
        QueueManager(const vk::PhysicalDevice&, const vk::SurfaceKHR&);
        QueueManager(QueueManager&&) = default;
        QueueManager& operator=(QueueManager&&) = default;

        // Disable copies
        QueueManager(const QueueManager&) = delete;
        QueueManager& operator=(const QueueManager&) = delete;

        void    ReserveQueue(const QueueType);
        void    PopulateQueues(const vk::Device&);
        const std::map<QueueType, QueueFamily> GetRequestedQueues() const;

        // NOTE: Queue must have been allocated
        vk::Queue&   operator[](const QueueType);

    };
}

#endif
