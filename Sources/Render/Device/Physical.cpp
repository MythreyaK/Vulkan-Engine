#include "Physical.hpp"
#include "DeviceExtensions.hpp"
#include "Queue/Queue.hpp"
#include "Logger.hpp"

#include <iostream>
#include <map>

namespace VULKAN_HPP_NAMESPACE {
    std::ostream& operator<< (std::ostream& stream, const vk::ConformanceVersion& v) {
        return stream
            << static_cast<int> (v.major)    << "."
            << static_cast<int> (v.minor)    << "."
            << static_cast<int> (v.subminor) << "."
            << static_cast<int> (v.patch);
    }
}

namespace Engine::Render::Device {

    PhysicalDevice::PhysicalDevice(int index, const vk::PhysicalDevice& phyDev, const vk::SurfaceKHR& surf) :
        index(index), hardwareDevice(phyDev), score(ScoreDevice(surf)), allSurfaceFormats(hardwareDevice.getSurfaceFormatsKHR(surf)),
        allPresentModes(hardwareDevice.getSurfacePresentModesKHR(surf))
    {
        auto const dev_extns{ hardwareDevice.enumerateDeviceExtensionProperties() };

        bool allFound{ true };

        for (const auto& req_ext : requiredDeviceExtensions) {
            bool foundThis{ false };

            for (const auto& i : dev_extns) {
                if (strncmp(i.extensionName, req_ext, 255) == 0) {
                    foundThis = true;
                    break;
                }
            }
            allFound &= foundThis;
        }

        const auto surfaceCapabs{ hardwareDevice.getSurfaceCapabilitiesKHR(surf) };

        // Pick the present modes and formats we require.
        // TODO: Settle with what is supported

        bool surfSupport{ false };

        for (const auto& i : allSurfaceFormats) {
            if (i.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear && i.format == vk::Format::eB8G8R8A8Unorm) {
                surfSupport = true;
                surfaceFormat = i;
            }
        }

        // If mailbox is not present, use FIFO
        bool mailbox{ std::find(
                allPresentModes.cbegin(), allPresentModes.cend(), vk::PresentModeKHR::eMailbox
            ) != allPresentModes.cend()
        };

        if (mailbox) presentMode = vk::PresentModeKHR::eMailbox;
        else presentMode = vk::PresentModeKHR::eFifo;

        presentSupport = allFound & !allPresentModes.empty() & surfSupport & (surfaceCapabs.minImageCount > 1);
    }


    const int PhysicalDevice::ScoreDevice(const vk::SurfaceKHR& surf) {

        using VK_PROP = vk::PhysicalDeviceProperties2;
        using VK_DRIVER = vk::PhysicalDeviceDriverProperties;

        int score_{ 0 };
        auto features{ hardwareDevice.getFeatures2().features };
        auto allProperties { hardwareDevice.getProperties2
            <vk::PhysicalDeviceProperties2, vk::PhysicalDeviceDriverProperties>()
        };

        const auto properties{ allProperties.get<VK_PROP>().properties };
        const auto driverInfo{ allProperties.get<VK_DRIVER>() };

        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            score_ += 100;
        }

        if (features.geometryShader) {
            score_ += 20;
        }

        LOGGER << "GPU: "               << properties.deviceName                 << '\n';
        LOGGER << "\t Type:           " << vk::to_string(properties.deviceType)  << '\n';
        LOGGER << "\t Texture limit:  " << properties.limits.maxImageDimension2D << '\n';
        LOGGER << "\t Driver name:    " << driverInfo.driverName                 << '\n';
        LOGGER << "\t Driver make:    " << vk::to_string(driverInfo.driverID)    << '\n';
        LOGGER << "\t Driver version: " << driverInfo.driverInfo                 << '\n';
        LOGGER << "\t Driver VKAPI:   " << driverInfo.conformanceVersion         << '\n';

        return score_;
    }


    const bool PhysicalDevice::operator>(const PhysicalDevice& other) {
        return score > other.score;
    }

    const bool PhysicalDevice::operator<(const PhysicalDevice& other) {
        return score < other.score;
    }


    PhysicalDevice PickDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
        std::map<int, PhysicalDevice> devices{};
        
        int index{ 0 };
        for (const auto& i : instance.enumeratePhysicalDevices()) {
            auto dev    { PhysicalDevice(index++, i, surface) };
            auto score  { dev.GetScore() };
            devices.emplace(score, std::move(dev));
        }

        for (auto i{ devices.rbegin() }; i != devices.crend(); ++i) {
            if (i->second.SupportsPresent()) {
                LOGGER << "Picked Device: \"" << i->second.Name() << "\"\n";
                return std::move(i->second);
            }
        }

        throw std::runtime_error("No Vulkan capable devices found.");
    }


    const vk::Extent2D PhysicalDevice::GetExtent2D(const vk::SurfaceKHR& surface) const {
        return hardwareDevice.getSurfaceCapabilitiesKHR(surface).currentExtent;
    }

    const std::string PhysicalDevice::Name() const {
        return hardwareDevice.getProperties2().properties.deviceName;
    }

    const bool PhysicalDevice::IsDiscrete() const {
        return hardwareDevice.getProperties2().properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
    }

    const int PhysicalDevice::Index() const {
        return index;
    }

    const int PhysicalDevice::GetScore() const {
        return score;
    }

    const bool PhysicalDevice::SupportsPresent() const {
        return presentSupport;
    }

    const vk::PhysicalDevice PhysicalDevice::Get() const {
        return hardwareDevice;
    }

    const vk::SurfaceFormatKHR PhysicalDevice::SurfaceFormat() const {
        return surfaceFormat;
    }

    const vk::PresentModeKHR PhysicalDevice::PresentMode() const {
        return presentMode;
    }
}

