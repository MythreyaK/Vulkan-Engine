#include "DebugUtils.hpp"

namespace Engine::Render::Debug {

    DebugMessenger CreateDebugMessenger(vk::Instance& inst, CallbackFuncion callback, void* userData) {
        const auto debCreateInfo{ vk::DebugUtilsMessengerCreateInfoEXT()
            .setPUserData(userData)
            .setPfnUserCallback(*callback)
            // MSevr::eInfo
            .setMessageSeverity(MSevr::eVerbose | MSevr::eWarning | MSevr::eError)
            .setMessageType(MType::eValidation | MType::eGeneral | MType::ePerformance)
        };

        return inst.createDebugUtilsMessengerEXTUnique(debCreateInfo);
    }
}
