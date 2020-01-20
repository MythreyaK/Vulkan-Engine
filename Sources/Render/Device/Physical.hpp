#ifndef PHYSICAL_RENDER_DEVICE_HPP

#define PHYSICAL_RENDER_DEVICE_HPP

#include "VKinclude/VKinclude.hpp"

#include <optional>

namespace Engine::Render::Device {

    class PhysicalDevice {

    private:
        using VKP = vk::PhysicalDevice;

        int     index{ -1 };
        VKP     hardwareDevice;
        int     score{ 0 };

        std::vector<vk::SurfaceFormatKHR>   allSurfaceFormats;
        std::vector<vk::PresentModeKHR>     allPresentModes;

        bool    presentSupport{ false };

        vk::SurfaceFormatKHR    surfaceFormat{};
        vk::PresentModeKHR      presentMode{};

        const int  ScoreDevice(const vk::SurfaceKHR&);
        const bool operator>(const PhysicalDevice&);
        const bool operator<(const PhysicalDevice&);

    public:
        PhysicalDevice(PhysicalDevice&&) = default;
        PhysicalDevice& operator=(PhysicalDevice&&) = default;
        PhysicalDevice(int index, const vk::PhysicalDevice&, const vk::SurfaceKHR& surface);

        const vk::Extent2D          GetExtent2D(const vk::SurfaceKHR& surface) const;
        const vk::PhysicalDevice    Get()               const;
        const vk::SurfaceFormatKHR  SurfaceFormat()     const;
        const vk::PresentModeKHR    PresentMode()       const; 
        const std::string           Name()              const;
        const bool                  IsDiscrete()        const;
        const int                   Index()             const;
        const int                   GetScore()          const;
        const bool                  SupportsPresent()   const;
    };


    PhysicalDevice PickDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface);
}

#endif // !PHYSICAL_RENDER_DEVICE_HPP
