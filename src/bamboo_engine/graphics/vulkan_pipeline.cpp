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
// Created by Leon Suchy on 12.08.20.
//

#include <fstream>
#include "vulkan_pipeline.hpp"

namespace bbge {

    std::array<std::string_view, enum_size + 1> shader_type_names {
        "fragment", "vertex", "geometry", "compute", "INVALID_SHADER_TYPE"
    };

    const std::string_view& to_string(shader_type t) {
        if (t >= shader_type::enum_size) return shader_type_names[shader_type::enum_size];
        return shader_type_names[t];
    }

    std::array<VkShaderStageFlagBits, enum_size + 1> shader_type_vk_conversion {
        VK_SHADER_STAGE_FRAGMENT_BIT, VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_GEOMETRY_BIT, VK_SHADER_STAGE_COMPUTE_BIT
    };

    VkShaderStageFlagBits to_vulkan(shader_type t) {
        if (t >= shader_type::enum_size) {
            throw std::logic_error("Vulkan does not support the requested shader type.");
        }
        return shader_type_vk_conversion[t];
    }

    result<std::vector<std::byte>, std::runtime_error> vulkan_pipeline::load_binary_file(const std::filesystem::path& p) {

        std::ifstream is(p, std::ifstream::in | std::ifstream::binary);
        if (is.bad()) {
            return std::runtime_error(fmt::format("Failed to read file '{}': {}.", p, std::strerror(errno)));
        }
        is.seekg(std::ifstream::end);
        auto length = is.tellg();
        is.seekg(std::ifstream::beg);
        std::vector<std::byte> result(length);
        is.read(reinterpret_cast<char*>(result.data()), length);
        result.resize(is.gcount()); // if only part of the file was read we truncate the buffer
        return result;
    }

    result<VkShaderModule, vulkan_error>
    vulkan_pipeline::create_shader_module(VkDevice dev, const std::vector<std::byte>& spir_v_bytecode, shader_type type) const {

        assert(dev);

        VkShaderModuleCreateInfo create_info { };
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = spir_v_bytecode.size(); // in bytes
        create_info.pCode = reinterpret_cast<const uint32_t*>(spir_v_bytecode.data());

        VkShaderModule module = VK_NULL_HANDLE;
        auto res = vkCreateShaderModule(dev, &create_info, nullptr, &module);
        if (res != VkResult::VK_SUCCESS) {
            return vulkan_error(fmt::format("Failed to create shader module of type '{}' for rendering pipeline '{}'", type, m_name), res);
        }

        return module;
    }

    vulkan_pipeline::vulkan_pipeline(
        std::string&& name, const vulkan_pipeline::shader_module_paths& module_paths,
        const rendering_pipeline_settings& settings,
        VkDevice dev)
      : m_name(std::move(name)) {

        auto frag_code = load_binary_file(module_paths.fragment_shader).or_throw();
        auto vert_code = load_binary_file(module_paths.vertex_shader).or_throw();

        // make sure this code doesn't leak API resources on throwing
        struct module_wrapper {
            VkDevice device;
            VkShaderModule module;
            ~module_wrapper() {
                vkDestroyShaderModule(device, module, nullptr);
            }
        };

        auto frag_module = module_wrapper { dev, create_shader_module(dev, frag_code, shader_type::fragment).or_throw() };
        auto vert_module = module_wrapper { dev, create_shader_module(dev, vert_code, shader_type::vertex).or_throw() };

        std::array shader_stage_creation_infos = {
            make_shader_stage_create_info(vert_module.module, shader_type::vertex),
            make_shader_stage_create_info(frag_module.module, shader_type::fragment)
        };

        // TODO: continue here with fixed functions, state descriptors and render passes.
    }

    VkPipelineShaderStageCreateInfo
    vulkan_pipeline::make_shader_stage_create_info(VkShaderModule module, shader_type type) const {
        VkPipelineShaderStageCreateInfo create_info { };
        create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        create_info.stage = to_vulkan(type);
        create_info.module = module;
        create_info.pName = m_name.c_str();
        return create_info;
    }
}
