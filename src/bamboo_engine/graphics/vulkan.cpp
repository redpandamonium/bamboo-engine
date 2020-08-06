// Bamboo Engine, a 3D game engine.
// Copyright (C) 2020 Leon Suchy
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Created by Leon Suchy on 04.08.20.
//

#include "vulkan.hpp"
#include <SDL2/SDL_vulkan.h>
#include "../util/logging.hpp"

namespace bbge {

    const std::unordered_map<VkResult, std::string> vulkan_utils::result_names = {
        { VK_SUCCESS, "VK_SUCCESS" },
        { VK_NOT_READY, "VK_NOT_READY" },
        { VK_TIMEOUT, "VK_TIMEOUT" },
        { VK_EVENT_SET, "VK_EVENT_SET" },
        { VK_EVENT_RESET, "VK_EVENT_RESET" },
        { VK_INCOMPLETE, "VK_INCOMPLETE" },
        { VK_ERROR_OUT_OF_HOST_MEMORY, "VK_ERROR_OUT_OF_HOST_MEMORY" },
        { VK_ERROR_OUT_OF_DEVICE_MEMORY, "VK_ERROR_OUT_OF_DEVICE_MEMORY" },
        { VK_ERROR_INITIALIZATION_FAILED, "VK_ERROR_INITIALIZATION_FAILED" },
        { VK_ERROR_DEVICE_LOST, "VK_ERROR_DEVICE_LOST" },
        { VK_ERROR_MEMORY_MAP_FAILED, "VK_ERROR_MEMORY_MAP_FAILED" },
        { VK_ERROR_LAYER_NOT_PRESENT, "VK_ERROR_LAYER_NOT_PRESENT" },
        { VK_ERROR_EXTENSION_NOT_PRESENT, "VK_ERROR_EXTENSION_NOT_PRESENT" },
        { VK_ERROR_FEATURE_NOT_PRESENT, "VK_ERROR_FEATURE_NOT_PRESENT" },
        { VK_ERROR_INCOMPATIBLE_DRIVER, "VK_ERROR_INCOMPATIBLE_DRIVER" },
        { VK_ERROR_TOO_MANY_OBJECTS, "VK_ERROR_TOO_MANY_OBJECTS" },
        { VK_ERROR_FORMAT_NOT_SUPPORTED, "VK_ERROR_FORMAT_NOT_SUPPORTED" },
        { VK_ERROR_FRAGMENTED_POOL, "VK_ERROR_FRAGMENTED_POOL" },
        { VK_ERROR_OUT_OF_POOL_MEMORY, "VK_ERROR_OUT_OF_POOL_MEMORY" },
        { VK_ERROR_INVALID_EXTERNAL_HANDLE, "VK_ERROR_INVALID_EXTERNAL_HANDLE" },
        { VK_ERROR_SURFACE_LOST_KHR, "VK_ERROR_SURFACE_LOST_KHR" },
        { VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR" },
        { VK_SUBOPTIMAL_KHR, "VK_SUBOPTIMAL_KHR" },
        { VK_ERROR_OUT_OF_DATE_KHR, "VK_ERROR_OUT_OF_DATE_KHR" },
        { VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR" },
        { VK_ERROR_VALIDATION_FAILED_EXT, "VK_ERROR_VALIDATION_FAILED_EXT" },
        { VK_ERROR_INVALID_SHADER_NV, "VK_ERROR_INVALID_SHADER_NV" },
        { VK_ERROR_FRAGMENTATION_EXT, "VK_ERROR_FRAGMENTATION_EXT" },
        { VK_ERROR_NOT_PERMITTED_EXT, "VK_ERROR_NOT_PERMITTED_EXT" }
    };

    const std::string vulkan_utils::invalid_name = "UNKNOWN RESULT TYPE";

    vulkan_instance::vulkan_instance(const std::string& app_name, version app_version)
        : m_application_info(), m_instance() {

        // app info
#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
        auto api_version = VK_API_VERSION_1_0;
#pragma clang diagnostic pop

        m_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        m_application_info.apiVersion = api_version;
        m_application_info.pEngineName = "Bamboo Engine";
        m_application_info.engineVersion = vulkan_utils::make_version(engine_version);
        m_application_info.pApplicationName = app_name.c_str();
        m_application_info.applicationVersion = vulkan_utils::make_version(app_version);

        // extensions
        print_available_extensions();
        std::vector<const char*> extensions = get_required_extensions();
        {
            // join with optional extensions
            std::vector<const char*> optional = get_optional_extensions();
            extensions.reserve(extensions.size() + optional.size());
            extensions.insert(extensions.end(), optional.begin(), optional.end());
        }

        // log used extensions
        if (!extensions.empty()) {
            SPDLOG_DEBUG("Using Vulkan extensions:");
            for (const char* ext : extensions) {
                SPDLOG_DEBUG("+ {}", ext);
            }
        }

        // layers
        std::vector<const char*> layers;
#ifndef NDEBUG
        SPDLOG_DEBUG("Vulkan validations layers are enabled.");
        layers = query_available_validation_layers();
#endif

        // log layers
        if (!layers.empty()) {
            SPDLOG_DEBUG("Using vulkan validation layers:");
            for (const char* layer : layers) {
                SPDLOG_DEBUG("+ {}", layer);
            }
        }

        // create info
        VkInstanceCreateInfo instance_create_info { };
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo = &m_application_info;
        instance_create_info.enabledExtensionCount = extensions.size();
        instance_create_info.ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data();
        instance_create_info.enabledLayerCount = layers.size();
        instance_create_info.ppEnabledLayerNames = layers.empty() ? nullptr : layers.data();

        // instance creation debug messenger
#ifndef NDEBUG

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "hicpp-signed-bitwise"

        constexpr VkDebugUtilsMessageTypeFlagsEXT all_message_types =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        constexpr VkDebugUtilsMessageSeverityFlagsEXT all_severities =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    #pragma clang diagnostic pop

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info { };
        debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_create_info.messageType = all_message_types;
        debug_create_info.messageSeverity = all_severities;
        debug_create_info.pUserData = nullptr;
        debug_create_info.pfnUserCallback = &vulkan_instance::debug_callback;
        instance_create_info.pNext = &debug_create_info;

#endif

        // creation
        auto res = vkCreateInstance(&instance_create_info, nullptr, &m_instance);
        if (res != VK_SUCCESS) {
            throw vulkan_error("Failed to create instance.", res);
        }

        SPDLOG_TRACE("Created Vulkan instance.");
    }

    vulkan_instance::~vulkan_instance() {
        vkDestroyInstance(m_instance, nullptr);

        SPDLOG_TRACE("Destroyed Vulkan instance.");
    }

    void vulkan_instance::print_available_extensions() {

        uint32_t count = 0;
        auto res = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            SPDLOG_WARN("Failed to query Vulkan instance extension properties ({}), but this wasn't in a critical context.", vulkan_utils::to_string(res));
            return;
        }
        std::vector<VkExtensionProperties> props(count);
        res = vkEnumerateInstanceExtensionProperties(nullptr, &count, props.data());
        if (res != VkResult::VK_SUCCESS) {
            SPDLOG_WARN("Failed to query Vulkan instance extension properties ({}), but this wasn't in a critical context.", vulkan_utils::to_string(res))  ;
            return;
        }

        SPDLOG_TRACE("Available Vulkan instance extensions: ");
        for (const auto& prop : props) {
            SPDLOG_TRACE("+ {} at version {}.", prop.extensionName, prop.specVersion);
        }
    }

    std::vector<const char*> vulkan_instance::get_optional_extensions() {
        return { };
    }

    std::vector<const char*> vulkan_instance::query_available_validation_layers() {

        uint32_t count = 0;
        auto res = vkEnumerateInstanceLayerProperties(&count, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            SPDLOG_WARN("Failed to query the available validation layers ({}). Not using any.", vulkan_utils::to_string(res));
            return { };
        }
        std::vector<VkLayerProperties> props(count);
        res = vkEnumerateInstanceLayerProperties(&count, props.data());
        if (res != VkResult::VK_SUCCESS) {
            SPDLOG_WARN("Failed to query the available validation layers ({}). Not using any.", vulkan_utils::to_string(res));
            return { };
        }

        std::vector<const char*> avail;

        for (const char* requested_layer : validation_layers) {
            auto it = std::find_if(props.begin(), props.end(),
                                   [requested_layer](const VkLayerProperties& p) { return std::strcmp(requested_layer, p.layerName) == 0; });
            if (it == props.end()) continue; // not available

            avail.push_back(requested_layer);
            SPDLOG_DEBUG("Using Vulkan validation layer {} at version {}.", it->layerName, it->specVersion);
        }

        return avail;
    }

    std::vector<const char*> vulkan_instance::get_required_extensions() {

        // GLFW
        uint32_t num_exts = 0;
        const char** exts = glfwGetRequiredInstanceExtensions(&num_exts);
        std::vector<const char*> result(num_exts);
        std::copy(exts, exts + num_exts, result.begin());

        // Check if they are available and throw if not
        uint32_t num_avail = 0;
        auto res = vkEnumerateInstanceExtensionProperties(nullptr, &num_avail, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            throw vulkan_error("Failed to query instance extensions", res);
        }
        std::vector<VkExtensionProperties> available(num_avail);
        res = vkEnumerateInstanceExtensionProperties(nullptr, &num_avail, available.data());
        if (res != VkResult::VK_SUCCESS) {
            throw vulkan_error("Failed to query instance extensions", res);
        }

        for (const char* ext : result) {
            auto it = std::find_if(available.begin(), available.end(),
                                   [ext](const VkExtensionProperties& props) { return std::strcmp(props.extensionName, ext) == 0; });
            if (it == available.end()) {
                throw vulkan_error(
                    fmt::format("Required extension {} not supported", ext),
                    VkResult::VK_ERROR_EXTENSION_NOT_PRESENT
                );
            }
        }

        return result;
    }

    VkBool32 vulkan_instance::debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data, [[maybe_unused]] void* user_data) {

        const char* type = vulkan_utils::convert_type(message_type);

        switch (message_severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                SPDLOG_TRACE("[{}]: {}.", type, callback_data->pMessage);
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                SPDLOG_DEBUG("[{}]: {}.", type, callback_data->pMessage);
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                SPDLOG_WARN("[{}]: {}.", type, callback_data->pMessage);
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                SPDLOG_ERROR("[{}]: {}.", type, callback_data->pMessage);
            default:
                SPDLOG_DEBUG("[{}]: {}.", type, callback_data->pMessage);
        }

        return VK_FALSE;
    }

    int vulkan_utils::make_version(const bbge::version& v) {

        // leaky macro
#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
        return VK_MAKE_VERSION(
            v.get_major(),
            v.has_minor() ? v.get_minor() : 0,
            v.has_patch() ? v.get_patch() : 0
        );
#pragma clang diagnostic pop
    }

    const std::string& vulkan_utils::to_string(VkResult res) {
        auto it = result_names.find(res);
        if (it == result_names.end()) return invalid_name;
        return it->second;
    }

    const char* vulkan_utils::convert_severity(VkDebugUtilsMessageSeverityFlagBitsEXT sev) {
        switch (sev) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                return "verbose";
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                return "info";
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                return "warning";
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                return "error";
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
                throw std::invalid_argument("VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT is not a valid severity.");
        }
    }

    const char* vulkan_utils::convert_type(VkDebugUtilsMessageTypeFlagsEXT typ) {
        switch (typ) {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                return "general";
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                return "validation";
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                return "perf";
            case VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT:
                throw std::invalid_argument("VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT is not a valid severity.");
            default:
                throw std::logic_error("Unhandled switch case.");
        }
    }

    vulkan_error::vulkan_error(const std::string& msg, VkResult res)
        : std::runtime_error(fmt::format("[vulkan] {} (err={}).", msg, vulkan_utils::to_string(res))) {

    }
}