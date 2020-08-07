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

#ifndef BAMBOOENGINE_VULKAN_UTILS_HPP
#define BAMBOOENGINE_VULKAN_UTILS_HPP

#include <vector>
#include <string>
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <tsl/robin_map.h>
#include "../util/result.hpp"
#include "../util/version.hpp"
#include "vulkan.hpp"

namespace bbge {

    struct vulkan_utils {
    public:

        // Enum conversions
        static int convert_version(const version& v);

        // Stringify
        static std::string_view to_string(VkResult res);
        static std::string_view to_string(VkFormat format);
        [[nodiscard]] static const char* convert_device_type(VkPhysicalDeviceType t);
        [[nodiscard]] static const char* convert_severity(VkDebugUtilsMessageSeverityFlagBitsEXT sev);
        [[nodiscard]] static const char* convert_type(VkDebugUtilsMessageTypeFlagsEXT typ);
        [[nodiscard]] static std::string get_name(VkPhysicalDevice dev);

        // Common defaults
        [[nodiscard]] static VkDebugUtilsMessengerCreateInfoEXT make_debug_messenger_all_messages() noexcept;
        [[nodiscard]] static std::vector<const char*>           get_validation_layers();

        // Property queries
        [[nodiscard]] static result<std::vector<VkExtensionProperties>, vulkan_error>   query_available_instance_extensions();
        [[nodiscard]] static result<std::vector<VkExtensionProperties>, vulkan_error>   query_available_device_extensions(VkPhysicalDevice dev);
        [[nodiscard]] static result<std::vector<VkLayerProperties>, vulkan_error>       query_available_layers();
        [[nodiscard]] static std::vector<VkQueueFamilyProperties>                       query_queue_families(VkPhysicalDevice dev);
        [[nodiscard]] static result<std::vector<VkPhysicalDevice>, vulkan_error>        query_physical_devices(VkInstance inst);
        [[nodiscard]] static result<std::vector<VkSurfaceFormatKHR>, vulkan_error>      query_surface_formats(VkPhysicalDevice dev, VkSurfaceKHR surface);
        [[nodiscard]] static result<std::vector<VkPresentModeKHR>, vulkan_error>        query_present_modes(VkPhysicalDevice dev, VkSurfaceKHR surface);
        [[nodiscard]] static result<VkSurfaceCapabilitiesKHR, vulkan_error>             query_surface_capabilities(VkPhysicalDevice dev, VkSurfaceKHR surface);

    private:

        static constexpr const char* validation_layers[] = {
            "VK_LAYER_KHRONOS_validation",
            "VK_LAYER_LUNARG_standard_validation",
            "VK_LAYER_LUNARG_core_validation",
            "VK_LAYER_LUNARG_parameter_validation",
            "VK_LAYER_LUNARG_object_tracker"
        };

        static const tsl::robin_map<VkResult, std::string> result_names;
        static const tsl::robin_map<VkFormat, std::string> format_names;
        static const std::string_view invalid_name;
    };
}


#endif //BAMBOOENGINE_VULKAN_UTILS_HPP
