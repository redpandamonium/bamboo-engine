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
// Created by Leon Suchy on 07.08.20.
//

#include "vulkan_utils.hpp"
#include <fmt/format.h>

namespace bbge {

    const tsl::robin_map<VkResult, std::string> vulkan_utils::result_names = {
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

    const std::string_view vulkan_utils::invalid_name = "UNKNOWN RESULT TYPE";

    int vulkan_utils::convert_version(const version& v) {
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

    std::string_view vulkan_utils::to_string(VkResult res) {
        auto it = result_names.find(res);
        if (it == result_names.end()) return invalid_name;
        return it->second;
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

    const char* vulkan_utils::convert_device_type(VkPhysicalDeviceType t) {
        switch (t) {
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                return "other";
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                return "integrated";
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                return "discrete";
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                return "virtual";
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
                return "CPU";
            case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
                throw std::invalid_argument("VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM is not a valid device type.");
            default:
                throw std::logic_error(fmt::format("Unhandled enum value {} at {}:{}", t, __FILE__, __LINE__));
        }
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

    std::string vulkan_utils::get_name(VkPhysicalDevice dev) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(dev, &props);
        return props.deviceName;
    }

    VkDebugUtilsMessengerCreateInfoEXT vulkan_utils::make_debug_messenger_all_messages() noexcept {

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

        return debug_create_info;
    }

    result<std::vector<VkExtensionProperties>, vulkan_error>
    vulkan_utils::query_available_instance_extensions() {

        uint32_t num_avail = 0;
        auto res = vkEnumerateInstanceExtensionProperties(nullptr, &num_avail, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            return vulkan_error("Failed to query available instance extensions", res);
        }
        std::vector<VkExtensionProperties> available(num_avail);
        res = vkEnumerateInstanceExtensionProperties(nullptr, &num_avail, available.data());
        if (res != VkResult::VK_SUCCESS) {
            return vulkan_error("Failed to query available instance extensions", res);
        }

        return available;
    }

    result<std::vector<VkLayerProperties>, vulkan_error>
    vulkan_utils::query_available_layers() {

        uint32_t count = 0;
        auto res = vkEnumerateInstanceLayerProperties(&count, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            return vulkan_error("Failed to query available validation layers", res);
        }
        std::vector<VkLayerProperties> props(count);
        res = vkEnumerateInstanceLayerProperties(&count, props.data());
        if (res != VkResult::VK_SUCCESS) {
            return vulkan_error("Failed to query available validation layers", res);
        }

        return props;
    }

    std::vector<VkQueueFamilyProperties>
    vulkan_utils::query_queue_families(VkPhysicalDevice dev) {

        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);
        std::vector<VkQueueFamilyProperties> props(count);
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, props.data());
        return props;
    }

    result<std::vector<VkPhysicalDevice>, vulkan_error>
    vulkan_utils::query_physical_devices(VkInstance inst) {

        uint32_t count = 0;
        auto res = vkEnumeratePhysicalDevices(inst, &count, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            return vulkan_error("Failed to query physical devices", res);
        }
        std::vector<VkPhysicalDevice> devices(count);
        res = vkEnumeratePhysicalDevices(inst, &count, devices.data());
        if (res != VkResult::VK_SUCCESS) {
            return vulkan_error("Failed to query physical devices", res);
        }

        return devices;
    }

    std::vector<const char*> vulkan_utils::get_validation_layers() {
        auto props = vulkan_utils::query_available_layers();
        if (props.is_err()) return { }; // forward error
        std::vector<const char*> avail;

        for (const char* requested_layer : validation_layers) {

            const auto names_match = [requested_layer](const VkLayerProperties& p) {
                return std::strcmp(requested_layer, p.layerName) == 0;
            };

            auto it = std::find_if(props.ok()->begin(), props.ok()->end(), names_match);
            if (it == props.ok()->end()) continue; // not available

            avail.push_back(requested_layer);
        }

        return avail;
    }

    result<std::vector<VkSurfaceFormatKHR>, vulkan_error>
    vulkan_utils::query_surface_formats(VkPhysicalDevice dev, VkSurfaceKHR surface) {

        uint32_t count = 0;
        auto res = vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &count, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(dev, &props);
            return vulkan_error(fmt::format("Failed to query surface formats for '{}'", props.deviceName), res);
        }
        std::vector<VkSurfaceFormatKHR> formats(count);
        res = vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &count, formats.data());
        if (res != VkResult::VK_SUCCESS) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(dev, &props);
            return vulkan_error(fmt::format("Failed to query surface formats for '{}'", props.deviceName), res);
        }

        return formats;
    }

    result<std::vector<VkPresentModeKHR>, vulkan_error>
    vulkan_utils::query_present_modes(VkPhysicalDevice dev, VkSurfaceKHR surface) {

        uint32_t count = 0;
        auto res = vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &count, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(dev, &props);
            return vulkan_error(fmt::format("Failed to query present modes for '{}'", props.deviceName), res);
        }
        std::vector<VkPresentModeKHR> modes(count);
        res = vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &count, modes.data());
        if (res != VkResult::VK_SUCCESS) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(dev, &props);
            return vulkan_error(fmt::format("Failed to query present modes for '{}'", props.deviceName), res);
        }

        return modes;
    }

    result<VkSurfaceCapabilitiesKHR, vulkan_error>
    vulkan_utils::query_surface_capabilities(VkPhysicalDevice dev, VkSurfaceKHR surface) {
        VkSurfaceCapabilitiesKHR capabilities;
        auto res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, surface, &capabilities);
        if (res != VkResult::VK_SUCCESS) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(dev, &props);
            return vulkan_error(fmt::format("Failed to query surface capabilities of '{}'", props.deviceName), res);
        }
        return capabilities;
    }

    result<std::vector<VkExtensionProperties>, vulkan_error>
    vulkan_utils::query_available_device_extensions(VkPhysicalDevice dev) {

        uint32_t count = 0;
        auto res = vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(dev, &props);
            return vulkan_error(fmt::format("Failed to query supported extensions for '{}'", props.deviceName), res);
        }
        std::vector<VkExtensionProperties> exts(count);
        res = vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, exts.data());
        if (res != VkResult::VK_SUCCESS) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(dev, &props);
            return vulkan_error(fmt::format("Failed to query supported extensions for '{}'", props.deviceName), res);
        }
        return exts;
    }
}
