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
#include "../util/rectangle.hpp"

namespace bbge {

    enum class rasterizer_mode : uint8_t {
        fill, line, point
    };

    enum class rasterizer_cull_mode : uint8_t {
        none, front, back, both
    };

    enum class rasterizer_front_face : uint8_t {
        clockwise, counter_clockwise
    };

    // TODO: documentation
    struct rasterizer_depth_bias {
        float const_factor;
        float clamp;
        float slope_factor;
    };

    enum shader_type {
        fragment, vertex, geometry, compute, enum_size
    };

    struct rendering_pipeline_settings {

        // input layout description

        // rasterizer
        drect                                viewport;                                           // Viewport, should probably fit the window dimensions
        rasterizer_mode                      rasterizer_mode = rasterizer_mode::fill;            // Rasterizer mode
        float                                render_width    = 1.0f;                             // Rasterizer point/line width
        rasterizer_cull_mode                 cull_mode       = rasterizer_cull_mode::back;       // Which side of a primitive should be culled
        rasterizer_front_face                front_face      = rasterizer_front_face::clockwise; // Which side of the primitive is the front
        bool                                 depth_clamp     = false;                            // If true the rasterizer will clamp the depth values of fragments outside the clipping planes
        std::optional<rasterizer_depth_bias> depth_bias      = { };                              // Allows to set the depth bias

        // multisampling

        // color blending
    };

    // vulkan conversions
    VkPolygonMode           to_vulkan(rasterizer_mode m);
    VkCullModeFlags         to_vulkan(rasterizer_cull_mode m);
    VkFrontFace             to_vulkan(rasterizer_front_face f);
    VkShaderStageFlagBits   to_vulkan(shader_type t);

    // string conversions
    std::string_view to_string(shader_type t);
    std::string_view to_string(rasterizer_mode m);
    std::string_view to_string(rasterizer_cull_mode m);
    std::string_view to_string(rasterizer_front_face m);

    class vulkan_pipeline {
    public:

        BBGE_NO_COPIES(vulkan_pipeline);

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

        ~vulkan_pipeline();

    private:

        std::string m_name;
        VkDevice m_device;
        VkPipelineLayout m_layout;

        [[nodiscard]] static result<std::vector<std::byte>, std::runtime_error> load_binary_file(const std::filesystem::path& p);
        [[nodiscard]] result<VkShaderModule, vulkan_error> create_shader_module(VkDevice dev, const std::vector<std::byte>& spir_v_bytecode, shader_type type) const;
        [[nodiscard]] VkPipelineShaderStageCreateInfo make_shader_stage_create_info(VkShaderModule module, shader_type type) const;
    };
}

#endif //BAMBOOENGINE_VULKAN_PIPELINE_HPP
