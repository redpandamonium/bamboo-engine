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
#include "../util/custom_formatters.hpp"

namespace bbge {

    constexpr std::array<std::string_view, 4> shader_type_names {
        "fragment", "vertex", "geometry", "compute"
    };

    std::string_view to_string(shader_type t) {
        auto v = static_cast<uint8_t>(t);
        return shader_type_names[v];
    }

    constexpr std::array<std::string_view, 3> rasterizer_mode_names {
        "fill", "line", "point"
    };

    std::string_view to_string(rasterizer_mode m) {
        auto v = static_cast<uint8_t>(m);
        return rasterizer_mode_names[v];
    }

    constexpr std::array<std::string_view, 4> rasterizer_cull_mode_names {
        "none", "front", "back", "both"
    };

    std::string_view to_string(rasterizer_cull_mode m) {
        auto v = static_cast<uint8_t>(m);
        return rasterizer_cull_mode_names[v];
    }

    constexpr std::array<std::string_view, 2> rasterizer_front_face_names {
        "clockwise", "counter-clockwise"
    };

    std::string_view to_string(rasterizer_front_face m) {
        auto v = static_cast<uint8_t>(m);
        return rasterizer_front_face_names[v];
    }

    constexpr std::array<VkShaderStageFlagBits, 4> shader_type_vk_conversion {
        VK_SHADER_STAGE_FRAGMENT_BIT, VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_GEOMETRY_BIT, VK_SHADER_STAGE_COMPUTE_BIT
    };

    VkShaderStageFlagBits to_vulkan(shader_type t) {
        auto v = static_cast<uint8_t>(t);
        return shader_type_vk_conversion[v];
    }

    constexpr std::array<VkPolygonMode, 3> polygon_mode_vk_conversion {
        VkPolygonMode::VK_POLYGON_MODE_FILL,
        VkPolygonMode::VK_POLYGON_MODE_LINE,
        VkPolygonMode::VK_POLYGON_MODE_POINT
    };

    VkPolygonMode to_vulkan(rasterizer_mode m) {
        auto v = static_cast<uint8_t>(m);
        return polygon_mode_vk_conversion[v];
    }

    constexpr std::array<VkCullModeFlagBits, 4> cull_mode_vk_conversion {
        VkCullModeFlagBits::VK_CULL_MODE_NONE,
        VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT,
        VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT,
        VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK
    };

    VkCullModeFlags to_vulkan(rasterizer_cull_mode m) {
        auto v = static_cast<uint8_t>(m);
        return cull_mode_vk_conversion[v];
    }

    constexpr std::array<VkFrontFace, 2> front_face_vk_conversion {
        VkFrontFace::VK_FRONT_FACE_CLOCKWISE,
        VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE
    };

    VkFrontFace to_vulkan(rasterizer_front_face f) {
        auto v = static_cast<uint8_t>(f);
        return front_face_vk_conversion[v];
    }

    result<std::vector<std::byte>, std::runtime_error> vulkan_pipeline::load_binary_file(const std::filesystem::path& p) {

        if (!std::filesystem::exists(p)) {
            return std::runtime_error(fmt::format("File '{}' doesn't exist.", p.string()));
        }

        std::ifstream is(p, std::ifstream::in | std::ifstream::binary);
        if (!is) {
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
        VkDevice dev, const vulkan_swap_chain& swap_chain)
      : m_name(std::move(name)), m_device(dev), m_swap_chain(swap_chain),
        m_layout(VK_NULL_HANDLE), m_render_pass(VK_NULL_HANDLE) {

        // pipeline layout, for uniform variables
        m_layout = create_pipeline_layout().or_throw();

        // render passes
        m_render_pass = create_simple_render_pass().or_throw();

        // pipeline
        m_pipeline = create_pipeline(module_paths, settings).or_throw();

        SPDLOG_TRACE("Created vulkan pipeline.");
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

    vulkan_pipeline::~vulkan_pipeline() {
        if (m_pipeline) {
            vkDestroyPipeline(m_device, m_pipeline, nullptr);
        }
        if (m_layout) {
            vkDestroyPipelineLayout(m_device, m_layout, nullptr);
        }
        if (m_render_pass) {
            vkDestroyRenderPass(m_device, m_render_pass, nullptr);
        }
        SPDLOG_TRACE("Destroyed vulkan pipeline.");
    }

    result<VkRenderPass, vulkan_error> vulkan_pipeline::create_simple_render_pass() const {

        VkAttachmentDescription color_attachment { };
        color_attachment.format = m_swap_chain.get_format();
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref { };
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription single_subpass { };
        single_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        single_subpass.colorAttachmentCount = 1;
        single_subpass.pColorAttachments = &color_attachment_ref;

        VkRenderPassCreateInfo render_pass_create_info { };
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = 1;
        render_pass_create_info.pAttachments = &color_attachment;
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &single_subpass;

        VkRenderPass pass;
        auto res = vkCreateRenderPass(m_device, &render_pass_create_info, nullptr, &pass);
        if (res != VK_SUCCESS) {
            return vulkan_error("Failed to create render pass", res);
        }

        return pass;
    }

    result<VkPipelineLayout, vulkan_error> vulkan_pipeline::create_pipeline_layout() const {

        assert(m_device);

        VkPipelineLayoutCreateInfo pipeline_layout_create_info { };
        pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        VkPipelineLayout layout;
        auto res = vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &layout);
        if (res != VkResult::VK_SUCCESS) {
            return vulkan_error("Failed to create Vulkan pipeline layout", res);
        }

        return layout;
    }

    result<VkPipeline, vulkan_error>
    vulkan_pipeline::create_pipeline(
        const shader_module_paths& module_paths,
        const rendering_pipeline_settings& settings
    ) const {

        // preconditions
        assert(m_device);
        assert(m_layout);
        assert(m_render_pass);

        // shader modules
        auto frag_code = load_binary_file(module_paths.fragment_shader);
        auto vert_code = load_binary_file(module_paths.vertex_shader);

        if (!frag_code) {
            return vulkan_error(
                fmt::format("Failed to load shader module code: {}", frag_code.err()->what()),
                VK_ERROR_UNKNOWN
            );
        }
        if (!vert_code) {
            return vulkan_error(
                fmt::format("Failed to load shader module code: {}", vert_code.err()->what()),
                VK_ERROR_UNKNOWN
            );
        }

        // RAII wrapper to ensure cleanup
        struct module_wrapper {
            VkDevice device;
            VkShaderModule module;
            ~module_wrapper() {
                vkDestroyShaderModule(device, module, nullptr);
            }
        };

        auto frag_module = module_wrapper { m_device, create_shader_module(m_device, *frag_code.ok(), shader_type::fragment).or_throw() };
        auto vert_module = module_wrapper { m_device, create_shader_module(m_device, *vert_code.ok(), shader_type::vertex).or_throw() };

        std::array shader_stage_creation_infos = {
            make_shader_stage_create_info(vert_module.module, shader_type::vertex),
            make_shader_stage_create_info(frag_module.module, shader_type::fragment)
        };

        // Empty for now. Will have buffer layout descriptors
        VkPipelineVertexInputStateCreateInfo vertex_input_info { };
        vertex_input_info.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount     = 0;
        vertex_input_info.pVertexBindingDescriptions        = nullptr; // Optional
        vertex_input_info.vertexAttributeDescriptionCount   = 0;
        vertex_input_info.pVertexAttributeDescriptions      = nullptr; // Optional

        // Triangle list
        VkPipelineInputAssemblyStateCreateInfo input_assembly { };
        input_assembly.sType                    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology                 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable   = VK_FALSE;

        // Viewport & scissor
        VkViewport viewport = to_vulkan_viewport(settings.viewport);
        VkRect2D scissor = {
            { static_cast<int32_t>(viewport.x), static_cast<int32_t>(viewport.y) },
            { static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height) }
        };

        // There is only one scissor and one viewport, so set the above
        VkPipelineViewportStateCreateInfo viewport_create_info { };
        viewport_create_info.sType          = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_create_info.viewportCount  = 1;
        viewport_create_info.pViewports     = &viewport;
        viewport_create_info.scissorCount   = 1;
        viewport_create_info.pScissors      = &scissor;

        // rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer_create_info { };
        rasterizer_create_info.sType            = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer_create_info.depthClampEnable = settings.depth_clamp;
        rasterizer_create_info.polygonMode      = to_vulkan(settings.rasterizer_mode);
        rasterizer_create_info.lineWidth        = settings.render_width;
        rasterizer_create_info.cullMode         = to_vulkan(settings.cull_mode);
        rasterizer_create_info.frontFace        = to_vulkan(settings.front_face);
        rasterizer_create_info.depthBiasEnable  = settings.depth_bias.has_value();
        if (settings.depth_bias) {
            rasterizer_create_info.depthBiasConstantFactor  = settings.depth_bias->const_factor;
            rasterizer_create_info.depthBiasClamp           = settings.depth_bias->clamp;
            rasterizer_create_info.depthBiasSlopeFactor     = settings.depth_bias->slope_factor;
        }

        // multisampling, empty for now
        VkPipelineMultisampleStateCreateInfo multisampling { };
        multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable   = VK_FALSE;
        multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading      = 1.0f;
        multisampling.pSampleMask           = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable      = VK_FALSE;

        #pragma clang diagnostic push
        #pragma ide diagnostic ignored "hicpp-signed-bitwise"
        static constexpr auto all_colors_components =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        #pragma clang diagnostic pop

        // color blending
        VkPipelineColorBlendAttachmentState color_blend_attachment { };
        color_blend_attachment.colorWriteMask       = all_colors_components;
        color_blend_attachment.blendEnable          = VK_TRUE;
        color_blend_attachment.srcColorBlendFactor  = VK_BLEND_FACTOR_SRC_ALPHA;
        color_blend_attachment.dstColorBlendFactor  = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        color_blend_attachment.colorBlendOp         = VK_BLEND_OP_ADD;
        color_blend_attachment.srcAlphaBlendFactor  = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstAlphaBlendFactor  = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment.alphaBlendOp         = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo color_blending { };
        color_blending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable     = VK_FALSE;
        color_blending.logicOp           = VK_LOGIC_OP_COPY;
        color_blending.attachmentCount   = 1;
        color_blending.pAttachments      = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.0f;
        color_blending.blendConstants[1] = 0.0f;
        color_blending.blendConstants[2] = 0.0f;
        color_blending.blendConstants[3] = 0.0f;

        // < depth and stencil testing setup here

        // put it all together
        VkGraphicsPipelineCreateInfo create_info { };
        create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        create_info.stageCount = shader_stage_creation_infos.size();
        create_info.pStages = shader_stage_creation_infos.data();
        create_info.pVertexInputState = &vertex_input_info;
        create_info.pViewportState = &viewport_create_info;
        create_info.pRasterizationState = &rasterizer_create_info;
        create_info.pMultisampleState = &multisampling;
        create_info.pDepthStencilState = nullptr;
        create_info.pColorBlendState = &color_blending;
        create_info.pDynamicState = nullptr;
        create_info.layout = m_layout;
        create_info.renderPass = m_render_pass;
        create_info.subpass = 0;

        create_info.basePipelineHandle = VK_NULL_HANDLE;
        create_info.basePipelineIndex = -1;

        // create
        VkPipeline pipeline;
        auto res = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline);
        if (res != VK_SUCCESS) {
            return vulkan_error("Failed to create graphics pipeline", res);
        }

        return pipeline;
    }
}
