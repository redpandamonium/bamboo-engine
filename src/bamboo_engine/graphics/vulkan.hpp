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

#ifndef BAMBOOENGINE_VULKAN_HPP
#define BAMBOOENGINE_VULKAN_HPP

#include <vulkan/vulkan.h>
#include <string>
#include <unordered_map>
#include "../util/version.hpp"
#include "../client/window.hpp"
#include <cmake_config.hpp>
#include <set>
#include "../util/macros.hpp"
#include "../util/result.hpp"

namespace bbge {

    struct vulkan_utils {
    public:

        static int make_version(const version& v);
        static std::string_view to_string(VkResult res);
        [[nodiscard]] static const char* convert_device_type(VkPhysicalDeviceType t);
        [[nodiscard]] static const char* convert_severity(VkDebugUtilsMessageSeverityFlagBitsEXT sev);
        [[nodiscard]] static const char* convert_type(VkDebugUtilsMessageTypeFlagsEXT typ);
        [[nodiscard]] static VkDebugUtilsMessengerCreateInfoEXT make_debug_messenger_all_messages() noexcept;
        [[nodiscard]] static std::vector<VkExtensionProperties> query_available_instance_extensions();
        [[nodiscard]] static std::vector<VkLayerProperties> query_available_layers();
        [[nodiscard]] static std::vector<VkQueueFamilyProperties> query_queue_families(VkPhysicalDevice dev);
        [[nodiscard]] static std::vector<VkPhysicalDevice> query_physical_devices(VkInstance inst);
        [[nodiscard]] static std::vector<const char*> query_available_validation_layers();

    private:

        static constexpr const char* validation_layers[] = {
            "VK_LAYER_KHRONOS_validation",
            "VK_LAYER_LUNARG_standard_validation",
            "VK_LAYER_LUNARG_core_validation",
            "VK_LAYER_LUNARG_parameter_validation",
            "VK_LAYER_LUNARG_object_tracker"
        };

        static const std::unordered_map<VkResult, std::string> result_names;
        static const std::string_view invalid_name;
    };

    /**
     * @brief Loads vulkan extension functions at runtime.
     * The member functions have the same name as their Vulkan counterparts.
     * For documentation directly refer to the Vulkan spec.
     * Any VkInstance parameters are removed from the interface.
     */
    struct vulkan_function_loader {
    public:

        /**
         * @brief Load functions.
         * @param instance Valid vulkan instance the functions are tied to.
         */
        explicit vulkan_function_loader(VkInstance instance);

        VkResult vkCreateDebugUtilsMessengerEXT(
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pMessenger);

        VkResult vkDestroyDebugUtilsMessengerEXT(
            VkDebugUtilsMessengerEXT messenger,
            const VkAllocationCallbacks* pAllocator);


    private:

        VkInstance m_instance; // bind all calls to the instance that resolved the functions pointers

        PFN_vkCreateDebugUtilsMessengerEXT m_vkCreateDebugUtilsMessengerEXT;
        PFN_vkDestroyDebugUtilsMessengerEXT m_vkDestroyDebugUtilsMessengerEXT;
    };

    struct vulkan_error : public std::runtime_error {
        vulkan_error(const std::string& msg, VkResult res);
    };

    class vulkan_instance {
    public:

        vulkan_instance(const std::string& app_name, version app_version);
        ~vulkan_instance();

        BBGE_NO_COPIES(vulkan_instance);

        /**
         * @brief Debug callback used by the Vulkan API for validation purposes.
         * @param message_severity Severity
         * @param message_type Type
         * @param callback_data Additional data
         * @param user_data User provided data, ignored
         * @return VK_FALSE
         * @note DO NOT CALL THIS! Vulkan API only.
         */
        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
            void* user_data);

        /**
         * @brief Get the vulkan handle.
         * @return Vulkan handle
         */
        [[nodiscard]] VkInstance get_handle() const noexcept;

    private:

        static constexpr const version engine_version { BAMBOOENGINE_VERSION_MAJOR, BAMBOOENGINE_VERSION_MINOR };
        static constexpr const char* optional_extensions[] = {
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        };

        VkApplicationInfo m_application_info;
        VkInstance m_instance;

        // extensions
        static void print_available_extensions();
        [[nodiscard]] static std::vector<const char*> get_required_extensions();
        [[nodiscard]] static std::vector<const char*> get_optional_extensions();

        // layers
        static void print_available_layers();
    };

    class vulkan_debug_messenger {
    public:

        explicit vulkan_debug_messenger(VkInstance instance);
        ~vulkan_debug_messenger();

        BBGE_NO_COPIES(vulkan_debug_messenger);

    private:

        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_handle;
    };

    class vulkan_device {
    public:

        /**
         * @brief Strategy pattern to pick a physical device.
         */
        struct selection_strategy {
            virtual ~selection_strategy() = default;
            [[nodiscard]] virtual result<VkPhysicalDevice, std::runtime_error> select(VkInstance instance) const = 0;
        };

        /**
         * @brief Default strategy
         */
        class default_selection_strategy final : public selection_strategy {
        public:

            [[nodiscard]] result<VkPhysicalDevice, std::runtime_error> select(VkInstance instance) const override;

        private:

            [[nodiscard]] static bool is_device_unsuitable(VkPhysicalDevice dev);
            [[nodiscard]] static int score_device(VkPhysicalDevice dev);
        };

        struct queue_handles {
            VkQueue graphics;
        };

        /**
         * @brief Create a vulkan device. Let the implementation pick a device.
         * @param inst Valid instance
         */
        explicit vulkan_device(VkInstance inst, const selection_strategy& strategy = selection_default);

        /**
         * @brief Create a vulkan device. Use the provided device.
         * @param inst Valid instance
         * @param dev Device to use
         */
        vulkan_device(VkInstance inst, VkPhysicalDevice dev);

        ~vulkan_device();

        BBGE_NO_COPIES(vulkan_device);

        /**
         * @brief Get the device handle
         * @return Device handle
         */
        [[nodiscard]] VkDevice get_handle() const noexcept;

        /**
         * @brief Get the queue handles of this device.
         * @return Queue handles
         */
        [[nodiscard]] const queue_handles& get_queues() const noexcept;

    private:

        struct queue_family_indices {
            uint32_t graphics;
        };

        static constexpr const char* validation_layers[] = {
            "VK_LAYER_KHRONOS_validation",
            "VK_LAYER_LUNARG_standard_validation",
            "VK_LAYER_LUNARG_core_validation",
            "VK_LAYER_LUNARG_parameter_validation",
            "VK_LAYER_LUNARG_object_tracker"
        };

        static const default_selection_strategy selection_default;
        static constexpr const float default_queue_priority = 1.0f;

        VkInstance m_instance;
        VkPhysicalDevice m_physical_device;
        VkDevice m_device;
        queue_handles m_queue_handles;

        [[nodiscard]] std::pair<VkDevice, queue_family_indices> create_device() const;
        [[nodiscard]] queue_family_indices get_required_queue_family_indices() const;
        [[nodiscard]] static std::vector<const char*> get_extensions();
        [[nodiscard]] queue_handles get_queue_handles(const queue_family_indices& indices) const;

        // logging
        void log_available_physical_devices() const;
        void log_selected_physical_device() const;
    };
}

#endif //BAMBOOENGINE_VULKAN_HPP
