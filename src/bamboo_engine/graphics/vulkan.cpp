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
#include "vulkan_utils.hpp"
#include "../client/glfw.hpp"

namespace bbge {

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
        m_application_info.engineVersion = vulkan_utils::convert_version(engine_version);
        m_application_info.pApplicationName = app_name.c_str();
        m_application_info.applicationVersion = vulkan_utils::convert_version(app_version);

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
            layers = vulkan_utils::get_validation_layers();
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

        auto exts_res = vulkan_utils::query_available_instance_extensions();
        exts_res.log_err();
        if (exts_res.is_err()) return;
        SPDLOG_TRACE("Available Vulkan instance extensions: ");
        for (const auto& prop : *exts_res.ok()) {
            SPDLOG_TRACE("+ {} at version {}.", prop.extensionName, prop.specVersion);
        }
    }

    std::vector<const char*> vulkan_instance::get_optional_extensions() {

        std::vector<const char*> result;
        auto available_res = vulkan_utils::query_available_instance_extensions();
        available_res.log_err();
        if (available_res.is_err()) return { };
        auto& available = *available_res.ok();

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
        auto available = std::move(vulkan_utils::query_available_instance_extensions().or_throw());

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
        auto layers_res = vulkan_utils::query_available_layers();
        layers_res.log_err();
        if (layers_res.is_err()) return;

        SPDLOG_TRACE("Available Vulkan instance layers:");
        for (const auto& layer : *layers_res.ok()) {
            SPDLOG_TRACE("+ {} at version {}.", layer.layerName, layer.specVersion);
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
        auto [device, q_fam_indices] = create_device();
        m_device = device;
        m_queue_family_indices = q_fam_indices;
        m_queue_handles = get_queue_handles(q_fam_indices);
    }

    result<VkPhysicalDevice, std::runtime_error>
    vulkan_device::default_selection_strategy::select(VkInstance instance, VkSurfaceKHR surface) const {

        // query devices
        auto devices_res = vulkan_utils::query_physical_devices(instance);
        if (devices_res.is_err()) return *devices_res.err();
        auto& devices = *devices_res.ok();
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

        if (!check_graphics_support(dev)) return false;
        if (!check_presentation_support(dev, surface)) return false;
        if (!check_required_extensions(dev)) return false;
        if (!check_swap_chain_support(surface, dev)) return false;

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

    bool vulkan_device::default_selection_strategy::check_swap_chain_support(
        VkSurfaceKHR surface, VkPhysicalDevice dev) {

        auto formats_res = vulkan_utils::query_surface_formats(dev, surface);
        if (formats_res.or_else({ }).empty()) return false;
        auto present_modes_res = vulkan_utils::query_present_modes(dev, surface);
        if (present_modes_res.or_else({ }).empty()) return false;

        return true;
    }

    bool vulkan_device::default_selection_strategy::check_required_extensions(VkPhysicalDevice dev) {

        auto extensions_res = vulkan_utils::query_available_device_extensions(dev);
        auto& extensions = extensions_res.or_throw();
        for (const char* required : required_extensions) {
            auto it = std::find_if(extensions.begin(), extensions.end(),
                                   [required](const VkExtensionProperties& props) { return std::strcmp(required, props.extensionName) == 0; });
            if (it == extensions.end()) {
                return false;
            }
        }

        return true;
    }

    bool vulkan_device::default_selection_strategy::check_presentation_support(VkPhysicalDevice dev, VkSurfaceKHR surface) {

        auto queue_families = vulkan_utils::query_queue_families(dev);

        for (auto it = queue_families.begin(); it != queue_families.end(); ++it) {
            VkBool32 supported = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(dev, std::distance(queue_families.begin(), it), surface, &supported);
            if (supported) return true;
        }

        return false;
    }

    bool vulkan_device::default_selection_strategy::check_graphics_support(VkPhysicalDevice dev) {

        const auto has_graphics_queue_family = [] (const VkQueueFamilyProperties& qf) {
            return qf.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        };

        // Check if all required queue families are supported
        auto queue_families = vulkan_utils::query_queue_families(dev);
        return std::any_of(queue_families.begin(), queue_families.end(), has_graphics_queue_family);
    }

    const vulkan_device::default_selection_strategy vulkan_device::selection_default { };

    vulkan_device::vulkan_device(VkInstance inst, VkSurfaceKHR surface, VkPhysicalDevice dev)
      : m_instance(inst), m_surface(surface), m_physical_device(dev), m_device(), m_queue_handles() {

        const auto [device, q_fam_indices] = create_device();
        m_device = device;
        m_queue_family_indices = q_fam_indices;
        m_queue_handles = get_queue_handles(q_fam_indices);
    }

    std::pair<VkDevice, vulkan_queue_family_indices> vulkan_device::create_device() const {

        log_selected_physical_device();

        vulkan_queue_family_indices q_fam_indices = get_required_queue_family_indices();
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
        layers = vulkan_utils::get_validation_layers();
        #endif

        auto extensions = get_extensions();
        log_device_extensions(extensions);

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

    vulkan_queue_family_indices vulkan_device::get_required_queue_family_indices() const {

        const auto has_graphics_queue_family = [] (const VkQueueFamilyProperties& qf) {
            return qf.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        };

        vulkan_queue_family_indices family_indices { };
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

    std::vector<const char*> vulkan_device::get_extensions() const {

        // in debug mode we double check the selection strategy's selection
        #ifndef NDEBUG
            auto extensions = vulkan_utils::query_available_device_extensions(m_physical_device);
            for (const char* required : required_extensions) {
                auto it = std::find_if(extensions.or_throw().begin(), extensions.or_throw().end(),
                                       [required](const VkExtensionProperties& props) { return std::strcmp(required, props.extensionName) == 0; });
                if (it == extensions.or_throw().end()) {
                    throw std::logic_error("The device selection strategy selected an unsuitable device. Not all required extensions are supported.");
                }
            }
        #endif

        std::vector<const char*> exts(sizeof(required_extensions));
        std::copy(required_extensions, required_extensions + sizeof(required_extensions), exts.begin());
        return exts;
    }

    void vulkan_device::log_available_physical_devices() const {

        auto devices = vulkan_utils::query_physical_devices(m_instance);
        devices.log_err();
        if (devices.is_err()) return;

        SPDLOG_TRACE("Available physical devices: ");
        for (const auto& device : *devices.ok()) {

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
    vulkan_device::get_queue_handles(const vulkan_queue_family_indices& indices) const {
        queue_handles handles { };
        vkGetDeviceQueue(m_device, indices.graphics, 0, &handles.graphics);
        vkGetDeviceQueue(m_device, indices.presentation, 0, &handles.presentation);
        return handles;
    }

    void vulkan_device::log_device_extensions(const std::vector<const char*>& exts) const {
        if (!exts.empty()) {
            SPDLOG_TRACE("Using the following Vulkan device extensions: ");
            for (auto extension : exts) {
                SPDLOG_TRACE("+ {}", extension);
            }
        }
    }

    VkPhysicalDevice vulkan_device::get_physical_device() const noexcept {
        return m_physical_device;
    }

    const vulkan_queue_family_indices& vulkan_device::get_queue_family_indices() const noexcept {
        return m_queue_family_indices;
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
        SPDLOG_TRACE("Created Vulkan surface.");
    }

    VkSurfaceKHR vulkan_surface::get_handle() const noexcept {
        return m_surface;
    }

    vulkan_swap_chain::vulkan_swap_chain(
        VkPhysicalDevice physical_device, VkDevice device,
        VkSurfaceKHR surface, const glfw_window& window,
        const vulkan_queue_family_indices& q_fam_indices)
      : m_device(device), m_handle(VK_NULL_HANDLE), m_image_views() {

        auto capabilities = vulkan_utils::query_surface_capabilities(physical_device, surface).or_throw();

        VkSwapchainCreateInfoKHR create_info { };
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        create_info.imageArrayLayers = 1;
        create_info.surface = surface;
        create_info.preTransform = capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // disable window transparency
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = VK_NULL_HANDLE;

        // image count
        auto image_count = std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);
        create_info.minImageCount = image_count;

        // present mode
        auto present_mode = pick_present_mode(physical_device, surface);
        create_info.presentMode = present_mode;

        // surface format
        m_format = pick_surface_format(physical_device, surface);
        create_info.imageFormat = m_format.format;
        create_info.imageColorSpace = m_format.colorSpace;

        // extent (surface size)
        auto extent = pick_swap_extent(physical_device, surface, window.get_handle());
        create_info.imageExtent = extent;

        // queue settings
        auto q_settings = pick_queue_settings(q_fam_indices);
        create_info.imageSharingMode        = q_settings.image_sharing_mode;
        create_info.queueFamilyIndexCount   = q_settings.queue_family_indices.size();
        create_info.pQueueFamilyIndices     = q_settings.queue_family_indices.empty() ? nullptr : q_settings.queue_family_indices.data();

        // create the swap chain
        auto res = vkCreateSwapchainKHR(device, &create_info, nullptr, &m_handle);
        if (res != VkResult::VK_SUCCESS) {
            throw vulkan_error("Failed to create swap chain", res);
        }

        SPDLOG_TRACE("Created Vulkan swap chain.");

        m_images = std::move(vulkan_utils::query_swapchain_images(m_device, m_handle).or_throw());

    }

    VkSurfaceFormatKHR vulkan_swap_chain::pick_surface_format(VkPhysicalDevice device, VkSurfaceKHR surface) {
        auto surface_formats = vulkan_utils::query_surface_formats(device, surface);
        auto& formats = surface_formats.or_throw();
        if (formats.empty()) {
            throw std::logic_error("The device selection strategy selected an unsuitable device. No surface formats are supported.");
        }

        for (const auto& format : formats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return format;
            }
        }

        return formats.front();
    }

    VkPresentModeKHR vulkan_swap_chain::pick_present_mode(VkPhysicalDevice device, VkSurfaceKHR surface) {
        auto present_modes = vulkan_utils::query_present_modes(device, surface);
        auto& modes = present_modes.or_throw();
        if (modes.empty()) {
            throw std::logic_error("No present modes even though VK_PRESENT_MODE_FIFO_KHR should be guaranteed.");
        }

        // prefer triple buffered
        auto it = std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_MAILBOX_KHR);
        if (it != modes.end()) return VK_PRESENT_MODE_MAILBOX_KHR;

        // vsync
        return VK_PRESENT_MODE_FIFO_KHR; // always available
    }

    VkExtent2D vulkan_swap_chain::pick_swap_extent(VkPhysicalDevice device, VkSurfaceKHR surface, GLFWwindow* win) {

        auto capabilities = vulkan_utils::query_surface_capabilities(device, surface).or_throw();

        // use automatic resolution pick
        if (capabilities.currentExtent.width != std::numeric_limits<decltype(capabilities.currentExtent.width)>::max()) {
            return capabilities.currentExtent;
        }

        // use manual resolution pick
        glm::ivec2 queried_from_window { };
        glfwGetWindowSize(win, &queried_from_window.x, &queried_from_window.y);

        if (queried_from_window.x < 0 || queried_from_window.y < 0) {
            throw glfw_error("Window size returned from glfwGetWindowSize is invalid.");
        }

        glm::uvec2 ext = queried_from_window;
        ext.x = std::clamp(ext.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        ext.y = std::clamp(ext.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return { ext.x, ext.y };
    }

    vulkan_swap_chain::~vulkan_swap_chain() {
        if (m_handle) {
            vkDestroySwapchainKHR(m_device, m_handle, nullptr);
            SPDLOG_TRACE("Destroyed Vulkan swap chain.");
        }
        for (auto view : m_image_views) {
            vkDestroyImageView(m_device, view, nullptr);
        }
    }

    vulkan_swap_chain::queue_settings
    vulkan_swap_chain::pick_queue_settings(const vulkan_queue_family_indices& q_fam_indices) {
        std::set<uint32_t> indices = { q_fam_indices.presentation, q_fam_indices.graphics };

        // if two queue families share the same physical queue we need to enable synchronization
        if (indices.size() < 2) {
            return queue_settings {
                VK_SHARING_MODE_CONCURRENT,
                { indices.begin(), indices.end() }
            };
        }

        // otherwise it's simpler and faster to disable synchronization
        return {
            VK_SHARING_MODE_EXCLUSIVE,
            { } // optional parameter can be left empty
        };
    }

    VkSwapchainKHR vulkan_swap_chain::get_handle() const noexcept {
        return m_handle;
    }

    const std::vector<VkImage>& vulkan_swap_chain::get_images() const noexcept {
        return m_images;
    }

    std::vector<VkImageView> vulkan_swap_chain::create_image_views() const {

        std::vector<VkImageView> views;
        views.reserve(m_images.size());

        for (auto img : m_images) {

            VkImageViewCreateInfo create_info { };
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = img;
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = m_format.format;
            create_info.components = vulkan_utils::make_identity_component_mapping();

            // color buffer target with no mipmapping
            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;

            VkImageView view;
            auto res = vkCreateImageView(m_device, &create_info, nullptr, &view);
            if (res != VkResult::VK_SUCCESS) {
                throw vulkan_error("Failed to create image view", res);
            }
            views.push_back(view);
        }

        return views;
    }
}