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

    const std::string_view vulkan_utils::invalid_name = "UNKNOWN RESULT TYPE";

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
        print_available_layers();
        std::vector<const char*> layers;
        #ifndef NDEBUG
            SPDLOG_DEBUG("Vulkan validations layers are enabled.");
            layers = vulkan_utils::query_available_validation_layers();
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
            auto debug_messenger_create_info = vulkan_utils::make_debug_messenger_all_messages();
            instance_create_info.pNext = &debug_messenger_create_info;
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
        SPDLOG_TRACE("Available Vulkan instance extensions: ");
        for (const auto& prop : vulkan_utils::query_available_instance_extensions()) {
            SPDLOG_TRACE("+ {} at version {}.", prop.extensionName, prop.specVersion);
        }
    }

    std::vector<const char*> vulkan_instance::get_optional_extensions() {

        std::vector<const char*> result;
        auto available = vulkan_utils::query_available_instance_extensions();

        for (const char* requested : optional_extensions) {
            auto it = std::find_if(available.begin(), available.end(),
                [requested](const VkExtensionProperties& props) { return std::strcmp(props.extensionName, requested) == 0; });
            if (it != available.end()) {
                result.push_back(requested);
            }
        }

        return result;
    }

    std::vector<const char*> vulkan_instance::get_required_extensions() {

        // GLFW
        uint32_t num_exts = 0;
        const char** exts = glfwGetRequiredInstanceExtensions(&num_exts);
        std::vector<const char*> result(num_exts);
        std::copy(exts, exts + num_exts, result.begin());

        // Check if they are available and throw if not
        auto available = vulkan_utils::query_available_instance_extensions();

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
                SPDLOG_TRACE("[VkVal] [{}]: {}.", type, callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                SPDLOG_DEBUG("[VkVal] [{}]: {}.", type, callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                SPDLOG_WARN("[VkVal] [{}]: {}.", type, callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                SPDLOG_ERROR("[VkVal] [{}]: {}.", type, callback_data->pMessage);
                break;
            default:
                SPDLOG_DEBUG("[VkVal] [{}]: {}.", type, callback_data->pMessage);
        }

        return VK_FALSE;
    }

    VkInstance vulkan_instance::get_handle() const noexcept {
        return m_instance;
    }

    void vulkan_instance::print_available_layers() {
        SPDLOG_TRACE("Available Vulkan instance layers:");
        for (const auto& layer : vulkan_utils::query_available_layers()) {
            SPDLOG_TRACE("+ {} at version {}.", layer.layerName, layer.specVersion);
        }
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

    std::string_view vulkan_utils::to_string(VkResult res) {
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

    std::vector<VkExtensionProperties> vulkan_utils::query_available_instance_extensions() {

        uint32_t num_avail = 0;
        auto res = vkEnumerateInstanceExtensionProperties(nullptr, &num_avail, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            throw vulkan_error("Failed to query available instance extensions", res);
        }
        std::vector<VkExtensionProperties> available(num_avail);
        res = vkEnumerateInstanceExtensionProperties(nullptr, &num_avail, available.data());
        if (res != VkResult::VK_SUCCESS) {
            throw vulkan_error("Failed to query available instance extensions", res);
        }

        return available;
    }

    std::vector<VkLayerProperties> vulkan_utils::query_available_layers() {

        uint32_t count = 0;
        auto res = vkEnumerateInstanceLayerProperties(&count, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            throw vulkan_error("Failed to query available validation layers", res);
        }
        std::vector<VkLayerProperties> props(count);
        res = vkEnumerateInstanceLayerProperties(&count, props.data());
        if (res != VkResult::VK_SUCCESS) {
            throw vulkan_error("Failed to query available validation layers", res);
        }

        return props;
    }

    std::vector<VkQueueFamilyProperties> vulkan_utils::query_queue_families(VkPhysicalDevice dev) {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);
        std::vector<VkQueueFamilyProperties> props(count);
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, props.data());
        return props;
    }

    std::vector<VkPhysicalDevice> vulkan_utils::query_physical_devices(VkInstance inst) {

        uint32_t count = 0;
        auto res = vkEnumeratePhysicalDevices(inst, &count, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            throw vulkan_error("Failed to query physical devices", res);
        }
        std::vector<VkPhysicalDevice> devices(count);
        res = vkEnumeratePhysicalDevices(inst, &count, devices.data());
        if (res != VkResult::VK_SUCCESS) {
            throw vulkan_error("Failed to query physical devices", res);
        }

        return devices;
    }

    std::vector<const char*> vulkan_utils::query_available_validation_layers() {
        auto props = vulkan_utils::query_available_layers();
        std::vector<const char*> avail;

        for (const char* requested_layer : validation_layers) {
            auto it = std::find_if(props.begin(), props.end(),
                                   [requested_layer](const VkLayerProperties& p) { return std::strcmp(requested_layer, p.layerName) == 0; });
            if (it == props.end()) continue; // not available

            avail.push_back(requested_layer);
        }

        return avail;
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

    vulkan_error::vulkan_error(const std::string& msg, VkResult res)
        : std::runtime_error(fmt::format("[vulkan] {} (err={}).", msg, vulkan_utils::to_string(res))) {

    }

    vulkan_function_loader::vulkan_function_loader(VkInstance instance) : m_instance(instance) {
        m_vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        m_vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    }

    VkResult vulkan_function_loader::vkCreateDebugUtilsMessengerEXT(
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pMessenger) {

        if (!m_vkCreateDebugUtilsMessengerEXT) return VkResult::VK_ERROR_EXTENSION_NOT_PRESENT;
        return m_vkCreateDebugUtilsMessengerEXT(m_instance, pCreateInfo, pAllocator, pMessenger);
    }

    VkResult vulkan_function_loader::vkDestroyDebugUtilsMessengerEXT(
        VkDebugUtilsMessengerEXT messenger,
        const VkAllocationCallbacks* pAllocator) {

        if (!m_vkDestroyDebugUtilsMessengerEXT) return VkResult::VK_ERROR_EXTENSION_NOT_PRESENT;
        m_vkDestroyDebugUtilsMessengerEXT(m_instance, messenger, pAllocator);
        return VK_SUCCESS;
    }

    vulkan_debug_messenger::~vulkan_debug_messenger() {
        auto vkDestroyDebugUtilsMessengerEXT =
            (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");

        if (vkDestroyDebugUtilsMessengerEXT && m_handle) {
            vkDestroyDebugUtilsMessengerEXT(m_instance, m_handle, nullptr);
            SPDLOG_TRACE("Disabled Vulkan debug messenger.");
        }
    }

    vulkan_debug_messenger::vulkan_debug_messenger(VkInstance instance)
      : m_instance(instance), m_handle(VK_NULL_HANDLE) {

        auto messenger_create_info = vulkan_utils::make_debug_messenger_all_messages();

        auto vkCreateDebugUtilsMessengerEXT =
            (PFN_vkCreateDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (!vkCreateDebugUtilsMessengerEXT) {
            SPDLOG_WARN("Tried to enable Vulkan debug messenger, the extension is not available.");
            return;
        }
        auto res = vkCreateDebugUtilsMessengerEXT(instance, &messenger_create_info, nullptr, &m_handle);
        if (res != VkResult::VK_SUCCESS) {
            SPDLOG_WARN("Tried to enable Vulkan debug messenger, but it failed (err={}).", vulkan_utils::to_string(res));
            return;
        }

        SPDLOG_TRACE("Enabled Vulkan debug messenger.");
    }

    vulkan_device::~vulkan_device() {
        if (m_device) {
            vkDestroyDevice(m_device, nullptr);
            SPDLOG_TRACE("Destroyed Vulkan device.");
        }
    }

    vulkan_device::vulkan_device(VkInstance inst, VkSurfaceKHR surface, const selection_strategy& strat)
      : m_instance(inst), m_surface(surface),
        m_physical_device(strat.select(inst, surface).or_throw()), m_device(),
        m_queue_handles() {

        log_available_physical_devices();
        auto [device, indices] = create_device();
        m_device = device;
        m_queue_handles = get_queue_handles(indices);
    }

    result<VkPhysicalDevice, std::runtime_error> vulkan_device::default_selection_strategy::select(VkInstance instance, VkSurfaceKHR surface) const {

        // query devices
        auto devices = vulkan_utils::query_physical_devices(instance);
        if (devices.empty()) {
            return { std::runtime_error("No devices connected.") };
        }

        const auto is_device_unsuitable = [&] (VkPhysicalDevice dev) { return !is_device_suitable(surface, dev); };

        // remove all devices that are not suitable
        devices.erase(std::remove_if(devices.begin(), devices.end(), is_device_unsuitable), devices.end());
        if (devices.empty()) {
            return { std::runtime_error("No devices are suitable.") };
        }

        // score devices
        std::vector<std::pair<int, VkPhysicalDevice>> scores;
        scores.reserve(devices.size());
        for (const auto& dev : devices) {
            int score = score_device(dev);
            scores.emplace_back(score, dev);
        }

        // select and return the best one
        auto it = std::max_element(scores.begin(), scores.end(),
            [] (const auto& a, const auto& b) { return a.first < b.first; });

        return { it->second };
    }

    bool vulkan_device::default_selection_strategy::is_device_suitable(VkSurfaceKHR surface, VkPhysicalDevice dev) {

        const auto has_graphics_queue_family = [] (const VkQueueFamilyProperties& qf) { return qf.queueFlags & VK_QUEUE_GRAPHICS_BIT; };

        // Check if all required queue families are supported
        auto queue_families = vulkan_utils::query_queue_families(dev);
        if (std::none_of(queue_families.begin(), queue_families.end(), has_graphics_queue_family)) return false;

        // needs presentation support
        VkBool32 supported = VK_FALSE;
        for (auto it = queue_families.begin(); it != queue_families.end(); ++it) {
            vkGetPhysicalDeviceSurfaceSupportKHR(dev, std::distance(queue_families.begin(), it), surface, &supported);
            if (supported) break;
        }
        if (!supported) return false;

        return true;
    }

    int vulkan_device::default_selection_strategy::score_device(VkPhysicalDevice dev) {

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(dev, &properties);

        int score = 0;

        // heavily favor discrete GPUs
        if (properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 10000;
        }

        score += properties.limits.maxImageDimension2D;

        return score;
    }

    const vulkan_device::default_selection_strategy vulkan_device::selection_default { };

    vulkan_device::vulkan_device(VkInstance inst, VkSurfaceKHR surface, VkPhysicalDevice dev)
      : m_instance(inst), m_surface(surface), m_physical_device(dev), m_device(), m_queue_handles() {

        const auto [device, q_fam_indices] = create_device();
        m_device = device;
        m_queue_handles = get_queue_handles(q_fam_indices);
    }

    std::pair<VkDevice, vulkan_device::queue_family_indices> vulkan_device::create_device() const {

        log_selected_physical_device();

        queue_family_indices q_fam_indices = get_required_queue_family_indices();
        std::set<uint32_t> queue_family_index_set {
            q_fam_indices.graphics,
            q_fam_indices.presentation
        };
        std::vector<VkDeviceQueueCreateInfo> q_create_infos;
        q_create_infos.reserve(queue_family_index_set.size());

        for (auto idx : queue_family_index_set) {
            VkDeviceQueueCreateInfo create { };
            create.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            create.pQueuePriorities = &default_queue_priority;
            create.queueCount = 1;
            create.queueFamilyIndex = idx;
            q_create_infos.push_back(create);
        }

        // This is necessary cause on MacOS Vulkan 1.1 is not supported yet.
        // Newer implementations ignore this.
        // It should match up with the instance layer list.
        std::vector<const char*> layers;
        #ifndef NDEBUG
        layers = vulkan_utils::query_available_validation_layers();
        #endif

        auto extensions = get_extensions();

        VkPhysicalDeviceFeatures features { };

        VkDeviceCreateInfo create_info { };
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount = q_create_infos.size();
        create_info.pQueueCreateInfos = q_create_infos.data();
        create_info.enabledLayerCount = layers.size();
        create_info.ppEnabledLayerNames = layers.data();
        create_info.enabledExtensionCount = extensions.size();
        create_info.ppEnabledExtensionNames = extensions.data();
        create_info.pEnabledFeatures = &features;

        VkDevice dev;
        auto res = vkCreateDevice(m_physical_device, &create_info, nullptr, &dev);
        if (res != VkResult::VK_SUCCESS) {
            throw vulkan_error("Failed to create logical device", res);
        }

        SPDLOG_TRACE("Created logical Vulkan device.");

        return { dev, q_fam_indices };
    }

    vulkan_device::queue_family_indices vulkan_device::get_required_queue_family_indices() const {

        const auto has_graphics_queue_family = [] (const VkQueueFamilyProperties& qf) {
            return qf.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        };

        queue_family_indices family_indices { };
        auto queue_families = vulkan_utils::query_queue_families(m_physical_device);

        // graphics family
        auto graphics_it = std::find_if(queue_families.begin(), queue_families.end(), has_graphics_queue_family);
        if (graphics_it == queue_families.end()) throw std::logic_error("The device selection strategy selected an unsuitable device.");
        family_indices.graphics = std::distance(queue_families.begin(), graphics_it);

        // presentation family
        for (auto it = queue_families.begin(); it != queue_families.end(); ++it) {
            VkBool32 supported = VK_FALSE;
            auto idx = std::distance(queue_families.begin(), it);
            vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, idx, m_surface, &supported);
            if (supported) {
                family_indices.presentation = idx;
                break;
            }
        }

        return family_indices;
    }

    std::vector<const char*> vulkan_device::get_extensions() {
        // TODO: implement
        return {};
    }

    void vulkan_device::log_available_physical_devices() const {

        SPDLOG_TRACE("Available physical devices: ");
        for (const auto& device : vulkan_utils::query_physical_devices(m_instance)) {

            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);

            SPDLOG_TRACE("+ [{}] '{}' with driver version {}.",
                         vulkan_utils::convert_device_type(properties.deviceType),
                         properties.deviceName,
                         properties.driverVersion);
        }
    }

    void vulkan_device::log_selected_physical_device() const {

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(m_physical_device, &properties);

        SPDLOG_DEBUG("Using {} physical device '{}' with driver version {}.",
            vulkan_utils::convert_device_type(properties.deviceType),
            properties.deviceName,
            properties.driverVersion);
    }

    VkDevice vulkan_device::get_handle() const noexcept {
        return m_device;
    }

    const vulkan_device::queue_handles& vulkan_device::get_queues() const noexcept {
        return m_queue_handles;
    }

    vulkan_device::queue_handles
    vulkan_device::get_queue_handles(const vulkan_device::queue_family_indices& indices) const {
        queue_handles handles { };
        vkGetDeviceQueue(m_device, indices.graphics, 0, &handles.graphics);
        vkGetDeviceQueue(m_device, indices.presentation, 0, &handles.presentation);
        return handles;
    }

    vulkan_surface::~vulkan_surface() {
        if (m_surface) {
            vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
            SPDLOG_TRACE("Destroyed Vulkan surface.");
        }
    }

    vulkan_surface::vulkan_surface(VkInstance instance, const glfw_window& win)
      : m_instance(instance), m_surface(VK_NULL_HANDLE) {
        auto res = glfwCreateWindowSurface(instance, win.get_handle(), nullptr, &m_surface);
        if (res != VkResult::VK_SUCCESS) {
            throw vulkan_error("Failed to create window surface", res);
        }
    }
}