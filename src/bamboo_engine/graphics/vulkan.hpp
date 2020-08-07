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

#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <cmake_config.hpp>
#include "../util/version.hpp"
#include "../client/window.hpp"
#include "../util/macros.hpp"
#include "../util/result.hpp"

namespace bbge {

    struct vulkan_error : public std::runtime_error {
        vulkan_error(const std::string& msg, VkResult res);
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
            [[nodiscard]] virtual result<VkPhysicalDevice, std::runtime_error> select(VkInstance instance, VkSurfaceKHR surface) const = 0;
        };

        /**
         * @brief Default strategy
         */
        class default_selection_strategy final : public selection_strategy {
        public:

            [[nodiscard]] result<VkPhysicalDevice, std::runtime_error> select(VkInstance instance, VkSurfaceKHR surface) const override;

        private:

            [[nodiscard]] static bool is_device_suitable(VkSurfaceKHR surface, VkPhysicalDevice dev);
            [[nodiscard]] static int score_device(VkPhysicalDevice dev);
            [[nodiscard]] static bool check_device_swap_chain_support(VkSurfaceKHR surface, VkPhysicalDevice dev);
        };

        struct queue_handles {
            VkQueue graphics;
            VkQueue presentation;
        };

        /**
         * @brief Create a vulkan device. Let the implementation pick a device.
         * @param inst Valid instance
         */
        explicit vulkan_device(VkInstance inst, VkSurfaceKHR surface, const selection_strategy& strategy = selection_default);

        /**
         * @brief Create a vulkan device. Use the provided device.
         * @param inst Valid instance
         * @param dev Device to use
         */
        vulkan_device(VkInstance inst, VkSurfaceKHR surface, VkPhysicalDevice dev);

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
            uint32_t graphics = std::numeric_limits<uint32_t>::max(); // the high value makes API calls fail if not assigned
            uint32_t presentation = std::numeric_limits<uint32_t>::max();
        };

        static constexpr const char* validation_layers[] = {
            "VK_LAYER_KHRONOS_validation",
            "VK_LAYER_LUNARG_standard_validation",
            "VK_LAYER_LUNARG_core_validation",
            "VK_LAYER_LUNARG_parameter_validation",
            "VK_LAYER_LUNARG_object_tracker"
        };

        static constexpr const char* required_extensions[] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        static const default_selection_strategy selection_default;
        static constexpr const float default_queue_priority = 1.0f;

        VkInstance m_instance;
        VkSurfaceKHR m_surface;
        VkPhysicalDevice m_physical_device;
        VkDevice m_device;
        queue_handles m_queue_handles;

        [[nodiscard]] std::pair<VkDevice, queue_family_indices> create_device() const;
        [[nodiscard]] queue_family_indices get_required_queue_family_indices() const;
        [[nodiscard]] std::vector<const char*> get_extensions() const;
        [[nodiscard]] queue_handles get_queue_handles(const queue_family_indices& indices) const;

        // logging
        void log_available_physical_devices() const;
        void log_selected_physical_device() const;
        void log_device_extensions(const std::vector<const char*>& exts) const;
    };

    class vulkan_surface {
    public:

        /**
         * Create a new vulkan surface for a GLFW window.
         * @param instance Vulkan instance
         * @param win Window
         */
        vulkan_surface(VkInstance instance, const glfw_window& win);

        ~vulkan_surface();

        /**
         * Get the surface handle
         * @return Surface handle
         */
        [[nodiscard]] VkSurfaceKHR get_handle() const noexcept;

    private:

        VkInstance m_instance;
        VkSurfaceKHR m_surface;
    };

    class vulkan_swap_chain {
    public:

        vulkan_swap_chain(VkInstance instance, VkDevice device, VkSurfaceKHR surface);

    private:


    };
}

#endif //BAMBOOENGINE_VULKAN_HPP
