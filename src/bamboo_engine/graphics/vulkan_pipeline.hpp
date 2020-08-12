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

#ifndef BAMBOOENGINE_VULKAN_PIPELINE_HPP
#define BAMBOOENGINE_VULKAN_PIPELINE_HPP

#include <filesystem>
#include <vector>
#include <cstddef>
#include <vulkan/vulkan.h>
#include "vulkan.hpp"
#include "../util/result.hpp"

namespace bbge {

    struct rendering_pipeline_settings {

    };

    enum shader_type {
        fragment, vertex, geometry, compute, enum_size
    };

    const std::string_view& to_string(shader_type t);
    VkShaderStageFlagBits to_vulkan(shader_type t);

    class vulkan_pipeline {
    public:

        struct shader_module_paths {
            std::filesystem::path vertex_shader;
            std::filesystem::path fragment_shader;
        };

        vulkan_pipeline(
            std::string&& name,
            const shader_module_paths& module_paths,
            const rendering_pipeline_settings& settings,
            VkDevice dev
        );

    private:

        std::string m_name;

        [[nodiscard]] static result<std::vector<std::byte>, std::runtime_error> load_binary_file(const std::filesystem::path& p);
        [[nodiscard]] result<VkShaderModule, vulkan_error> create_shader_module(VkDevice dev, const std::vector<std::byte>& spir_v_bytecode, shader_type type) const;
        [[nodiscard]] VkPipelineShaderStageCreateInfo make_shader_stage_create_info(VkShaderModule module, shader_type type) const;
    };
}

#endif //BAMBOOENGINE_VULKAN_PIPELINE_HPP
