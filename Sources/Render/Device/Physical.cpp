#include "Physical.hpp"

#include "DeviceExtensions.hpp"

#include <iostream>
#include <map>


namespace Engine::Render::Device {


    const std::string StringifyDeviceType(const vk::PhysicalDeviceType& dt);
    const std::string StringifyQueueType(const vk::QueueFlags& qf);

    PhysicalDevice::PhysicalDevice(int index, const vk::PhysicalDevice& phy_dev, const vk::SurfaceKHR& surf) {
        this->index = index;
        hardwareDevice = phy_dev;

        auto& features      = hardwareDevice.getFeatures2().features;
        auto& properties    = hardwareDevice.getProperties2().properties;

        name = std::string{ properties.deviceName };

        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            isDiscrete = true;
            score += 100;
        }

        if (features.geometryShader) {
            score += 20;
        }

#       ifndef NDEBUG
        LOG << "GPU: "          << properties.deviceName << '\n';
        LOG << "\t Type: "      << StringifyDeviceType(properties.deviceType) << '\n';
        LOG << "\t Texture limit: " << properties.limits.maxImageDimension2D << '\n';
        LOG << "\t Device Extensions:\n";
#       endif // !NDEBUG

        // Now, update the queue details
        GetDeviceQueueInfo(surf);

        // Get all swapchain details
        presentSupport = GetSwapchainSupport(surf);

    }


    void PhysicalDevice::GetDeviceQueueInfo(const vk::SurfaceKHR& surface) {

        auto& queueFams = hardwareDevice.getQueueFamilyProperties2();

        int queueInx = -1;
        for (auto& queueFam : queueFams) {
            queueInx++;

            const auto& flags = queueFam.queueFamilyProperties.queueFlags;
            const auto& count = queueFam.queueFamilyProperties.queueCount;

            bool surfacePresentSupport = hardwareDevice.getSurfaceSupportKHR(queueInx, surface);
            uint32_t queueCountLeft{ count }; // queue count left in this family

            // if the queue count is 2, but supports all queue flags, we can't use any more
            // queues from this family
            // Assume queueCountLeft = 1. Then 'queueCountLeft--' is evaluated as '1' which evaluates to true.

            if (!qfGraphics.has_value() && (flags & vk::QueueFlagBits::eGraphics) && queueCountLeft--)
                qfGraphics = queueInx;

            if (!qfCompute.has_value() && (flags & vk::QueueFlagBits::eCompute) && queueCountLeft--)
                qfCompute = queueInx;

            if (!qfTransfer.has_value() && (flags & vk::QueueFlagBits::eTransfer) && queueCountLeft--)
                qfTransfer = queueInx;

            if (surfacePresentSupport && qfGraphics.has_value()) {
                qfPresent = queueInx;
                presentSupport = true;
            }

#           ifndef NDEBUG
            LOG << "\t Queue Familiy [" << queueInx << "]: \n";
            LOG << "\t\t Present Support: " << std::boolalpha << presentSupport << '\n';
            LOG << "\t\t Queue Count: " << count << '\n';
            LOG << "\t\t Queue flags: " << StringifyQueueType(queueFam.queueFamilyProperties.queueFlags) << '\n';
#           endif // !NDEBUG
        }
    }


    const bool PhysicalDevice::GetSwapchainSupport(const vk::SurfaceKHR& surf) {
        auto const& dev_extns = hardwareDevice.enumerateDeviceExtensionProperties();

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

        const auto& surfaceCapabs       = hardwareDevice.getSurfaceCapabilitiesKHR(surf);
        const auto& surfaceFormats      = hardwareDevice.getSurfaceFormatsKHR(surf);
        const auto& surfacePresentModes = hardwareDevice.getSurfacePresentModesKHR(surf);

        allSurfaceFormats   = surfaceFormats;
        allPresentModes     = surfacePresentModes;
        // Pick the present modes and formats we require. 
        // TODO: Settle with what is supported

        bool surfSupport{ false };
        bool presentModeSupport{ false };

        for (const auto& i : surfaceFormats) {
            if (i.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear && i.format == vk::Format::eB8G8R8A8Unorm) {
                surfSupport = true;
                surfaceFormat = i;
            }
        }

        // If mailbox is not present, use FIFO
        bool mailbox = std::find(
            surfacePresentModes.cbegin(), surfacePresentModes.cend(), vk::PresentModeKHR::eMailbox
        ) != surfacePresentModes.cend();

        if (mailbox) presentMode = vk::PresentModeKHR::eMailbox;
        else presentMode = vk::PresentModeKHR::eFifo;

        return allFound & !surfacePresentModes.empty() & surfSupport & (surfaceCapabs.minImageCount > 1);

    }


    const bool PhysicalDevice::operator>(const PhysicalDevice& other) {
        return score > other.score;
    }

    const bool PhysicalDevice::operator<(const PhysicalDevice& other) {
        return score < other.score;
    }


    const PhysicalDevice PickDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
        std::map<int, const PhysicalDevice> devices{};

        int index = 0;
        for (const auto& i : instance.enumeratePhysicalDevices()) {
            const auto& dev = PhysicalDevice(index, i, surface);
            devices.emplace(dev.GetScore(), dev);
        }

        for (auto& i = devices.crbegin(); i != devices.crend(); ++i) {
            if (i->second.SupportsPresent()) {
#               ifndef NDEBUG
                LOG << "Picked Device: \"" << i->second.Name() << "\"\n";
#               endif // !NDEBUG
                return i->second;
            }
        }

        throw std::runtime_error("No Vulkan capable devices found.");
    }


    const vk::Extent2D PhysicalDevice::GetExtent2D(const vk::SurfaceKHR& surface) {
        return hardwareDevice.getSurfaceCapabilitiesKHR(surface).currentExtent;
    }


    const std::string StringifyDeviceType(const vk::PhysicalDeviceType& dt) {
        using Phy_Type = vk::PhysicalDeviceType;
        if (dt == Phy_Type::eCpu)           return "CPU";
        if (dt == Phy_Type::eVirtualGpu)    return "Virtual GPU";
        if (dt == Phy_Type::eDiscreteGpu)   return "Discrete GPU";
        if (dt == Phy_Type::eIntegratedGpu) return "Integrated GPU";

        return "Unknown Type";
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


