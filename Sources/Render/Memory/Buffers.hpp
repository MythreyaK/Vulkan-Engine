#ifndef ENGINE_MEMORY_BUFFERS_HPP
#define ENGINE_MEMORY_BUFFERS_HPP

#include "VKinclude/VKinclude.hpp"
#include "Device/Physical.hpp"

namespace Engine::Render::Memory {

    template <typename T>
    class DeviceMemory {
    private:
        vk::MemoryPropertyFlags usageFlags;
        vk::UniqueBuffer        buffer;
        vk::MemoryRequirements  info;
        std::vector<T>          stagingBuffer;
        vk::UniqueDeviceMemory  memory;

        T* mappedPointer{ nullptr };

        uint32_t FindSuitable(const vk::PhysicalDeviceMemoryProperties& deviceMemProps, const uint32_t bitFlags);

    public:
        DeviceMemory() = default;
        DeviceMemory(const vk::Device&, const Engine::Render::Device::PhysicalDevice&, const vk::BufferCreateInfo&, const vk::MemoryPropertyFlags&);

        // No Copies
        DeviceMemory(const DeviceMemory&) = delete;
        DeviceMemory& operator=(const DeviceMemory&) = delete;

        // Moves allowed
        DeviceMemory(DeviceMemory&&) = default;
        DeviceMemory& operator=(DeviceMemory&&) = default;

        const uint32_t Size() const { return stagingBuffer.size(); }
        const vk::Buffer* Buffer() const { return &(buffer.get()); }
        std::vector<T>& StagingBuffer();

        void Map(const vk::Device&);
        void Unmap(const vk::Device&);
        void Flush(const vk::Device&);
    };


    // Definitions
    namespace ERD = Engine::Render::Device;

    template <typename T>
    DeviceMemory<T>::DeviceMemory(const vk::Device& renderDevice, const ERD::PhysicalDevice& phyDev, const vk::BufferCreateInfo& createInfo, const vk::MemoryPropertyFlags& properties) :
        usageFlags(properties),
        buffer(renderDevice.createBufferUnique(createInfo)),
        info(renderDevice.getBufferMemoryRequirements(buffer.get())),
        stagingBuffer(0) {

        const auto memoryIndex{ FindSuitable(
                phyDev.Get().getMemoryProperties2().memoryProperties,
                info.memoryTypeBits)
        };

        memory = renderDevice.allocateMemoryUnique(vk::MemoryAllocateInfo()
            .setAllocationSize(info.size)
            .setMemoryTypeIndex(memoryIndex)
        );

        renderDevice.bindBufferMemory(buffer.get(), memory.get(), 0u);
    }

    template <typename T>
    std::vector<T>& DeviceMemory<T>::StagingBuffer() {
        return stagingBuffer;
    }

    template <typename T>
    void DeviceMemory<T>::Map(const vk::Device& renderDevice) {
        void* data;
        renderDevice.mapMemory(memory.get(), 0u, info.size, {}, &data);
        std::memcpy(data, stagingBuffer.data(), info.size);
    }

    template <typename T>
    void DeviceMemory<T>::Unmap(const vk::Device& renderDevice) {
        renderDevice.unmapMemory(memory.get());
        mappedPointer = nullptr;
    }

    template <typename T>
    void DeviceMemory<T>::Flush(const vk::Device& renderDevice) {
        if (usageFlags & vk::MemoryPropertyFlagBits::eHostCoherent) { return; }
        else { throw std::runtime_error("Unhandled memory flush"); }
        // TODO: renderDevice.flushMappedMemoryRanges(0, vk::MappedMemoryRange().set)
    }

    template <typename T>
    uint32_t DeviceMemory<T>::FindSuitable(const vk::PhysicalDeviceMemoryProperties& deviceMemProps, const uint32_t bitFlags) {


        // Pick the first suitable type
        for (uint32_t i = 0; i < deviceMemProps.memoryTypeCount; ++i) {
            /*
            * Why? Quoting from the spec, at
            * https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#resources-association
            *
            * Under 'VkMemoryRequirements'
            *
            * "memoryTypeBits is a bitmask and contains one bit set for every supported memory
            * type for the resource. Bit i is set if and only if the memory type i in the
            * VkPhysicalDeviceMemoryProperties structure for the physical device is supported
            * for the resource."
            *
            * So we just check if bit i is set in bitFlags, starting from LSB.
            * Higher bits are 'less ideal' for performance reasons.
            */
            if ((bitFlags & (1 << i)) && ((deviceMemProps.memoryTypes[i].propertyFlags & usageFlags) == usageFlags)) {
                return i;
            }
        }

        // None available
        throw std::runtime_error("No type memory found");
    }
}

#endif
