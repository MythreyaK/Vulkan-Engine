#ifndef PHYSICAL_RENDER_DEVICE_HPP

#define PHYSICAL_RENDER_DEVICE_HPP

#include "VKinclude/VKinclude.hpp"

#include <optional>

namespace Engine::Render::Device {

    class PhysicalDevice {

    private:
        int             index{ -1 };
        int             score{ 0 };
        bool            isDiscrete{ false };
        bool            presentSupport{ false };
        std::string     name{};

        vk::PhysicalDevice                  hardwareDevice;

        vk::SurfaceFormatKHR                surfaceFormat{};
        vk::PresentModeKHR                  presentMode{};
         // TODO: This approach is clumsy. Think of an alternative way.
        std::optional<uint32_t>             qfPresent{};
        std::optional<uint32_t>             qfCompute{};
        std::optional<uint32_t>             qfGraphics{};
        std::optional<uint32_t>             qfTransfer{};

        std::vector<vk::SurfaceFormatKHR>   allSurfaceFormats;
        std::vector<vk::PresentModeKHR>     allPresentModes;

        const bool GetSwapchainSupport(const vk::SurfaceKHR&);
        void GetDeviceQueueInfo(const vk::SurfaceKHR&);
        const bool operator>(const PhysicalDevice& other);
        const bool operator<(const PhysicalDevice& other);

    public:
        PhysicalDevice() = default;
        PhysicalDevice(const PhysicalDevice&) = default;
        PhysicalDevice(PhysicalDevice&&) = default;
        PhysicalDevice(int index, const vk::PhysicalDevice&, const vk::SurfaceKHR& surface);

        PhysicalDevice& operator=(const PhysicalDevice&) = default;
        PhysicalDevice& operator=(PhysicalDevice&&) = default;

        const int  GetIndex()          const { return index;                }
        const int  GetScore()          const { return score;                }
        const int  IsDiscrete()        const { return isDiscrete;           }
        const std::string Name()       const { return name;                 }
        const bool SupportsPresent()   const { return presentSupport;       }
        const bool SupportsCompute()   const { return qfCompute.has_value();}
        const bool SupportsTransfer()  const { return presentSupport;       }
        const uint32_t PresentQueue()  const { return qfPresent.value();    }
        const uint32_t GraphicsQueue() const { return qfGraphics.value();   }
        const vk::PhysicalDevice Get() const { return hardwareDevice;       }

        const vk::SurfaceFormatKHR  SurfaceFormat() const { return surfaceFormat; }
        const vk::PresentModeKHR    PresentMode()   const { return presentMode; }
        const vk::Extent2D GetExtent2D(const vk::SurfaceKHR& surface);

    };


    const PhysicalDevice PickDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface);
}

#endif // !PHYSICAL_RENDER_DEVICE_HPP
