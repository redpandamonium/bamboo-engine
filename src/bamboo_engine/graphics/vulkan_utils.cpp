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

    const tsl::robin_map<VkFormat, std::string> vulkan_utils::format_names = {
        { VK_FORMAT_UNDEFINED, "VK_FORMAT_UNDEFINED" },
        { VK_FORMAT_R4G4_UNORM_PACK8, "VK_FORMAT_R4G4_UNORM_PACK8" },
        { VK_FORMAT_R4G4B4A4_UNORM_PACK16, "VK_FORMAT_R4G4B4A4_UNORM_PACK16" },
        { VK_FORMAT_B4G4R4A4_UNORM_PACK16, "VK_FORMAT_B4G4R4A4_UNORM_PACK16" },
        { VK_FORMAT_R5G6B5_UNORM_PACK16, "VK_FORMAT_R5G6B5_UNORM_PACK16" },
        { VK_FORMAT_B5G6R5_UNORM_PACK16, "VK_FORMAT_B5G6R5_UNORM_PACK16" },
        { VK_FORMAT_R5G5B5A1_UNORM_PACK16, "VK_FORMAT_R5G5B5A1_UNORM_PACK16" },
        { VK_FORMAT_B5G5R5A1_UNORM_PACK16, "VK_FORMAT_B5G5R5A1_UNORM_PACK16" },
        { VK_FORMAT_A1R5G5B5_UNORM_PACK16, "VK_FORMAT_A1R5G5B5_UNORM_PACK16" },
        { VK_FORMAT_R8_UNORM, "VK_FORMAT_R8_UNORM" },
        { VK_FORMAT_R8_SNORM, "VK_FORMAT_R8_SNORM" },
        { VK_FORMAT_R8_USCALED, "VK_FORMAT_R8_USCALED" },
        { VK_FORMAT_R8_SSCALED, "VK_FORMAT_R8_SSCALED" },
        { VK_FORMAT_R8_UINT, "VK_FORMAT_R8_UINT" },
        { VK_FORMAT_R8_SINT, "VK_FORMAT_R8_SINT" },
        { VK_FORMAT_R8_SRGB, "VK_FORMAT_R8_SRGB" },
        { VK_FORMAT_R8G8_UNORM, "VK_FORMAT_R8G8_UNORM" },
        { VK_FORMAT_R8G8_SNORM, "VK_FORMAT_R8G8_SNORM" },
        { VK_FORMAT_R8G8_USCALED, "VK_FORMAT_R8G8_USCALED" },
        { VK_FORMAT_R8G8_SSCALED, "VK_FORMAT_R8G8_SSCALED" },
        { VK_FORMAT_R8G8_UINT, "VK_FORMAT_R8G8_UINT" },
        { VK_FORMAT_R8G8_SINT, "VK_FORMAT_R8G8_SINT" },
        { VK_FORMAT_R8G8_SRGB, "VK_FORMAT_R8G8_SRGB" },
        { VK_FORMAT_R8G8B8_UNORM, "VK_FORMAT_R8G8B8_UNORM" },
        { VK_FORMAT_R8G8B8_SNORM, "VK_FORMAT_R8G8B8_SNORM" },
        { VK_FORMAT_R8G8B8_USCALED, "VK_FORMAT_R8G8B8_USCALED" },
        { VK_FORMAT_R8G8B8_SSCALED, "VK_FORMAT_R8G8B8_SSCALED" },
        { VK_FORMAT_R8G8B8_UINT, "VK_FORMAT_R8G8B8_UINT" },
        { VK_FORMAT_R8G8B8_SINT, "VK_FORMAT_R8G8B8_SINT" },
        { VK_FORMAT_R8G8B8_SRGB, "VK_FORMAT_R8G8B8_SRGB" },
        { VK_FORMAT_B8G8R8_UNORM, "VK_FORMAT_B8G8R8_UNORM" },
        { VK_FORMAT_B8G8R8_SNORM, "VK_FORMAT_B8G8R8_SNORM" },
        { VK_FORMAT_B8G8R8_USCALED, "VK_FORMAT_B8G8R8_USCALED" },
        { VK_FORMAT_B8G8R8_SSCALED, "VK_FORMAT_B8G8R8_SSCALED" },
        { VK_FORMAT_B8G8R8_UINT, "VK_FORMAT_B8G8R8_UINT" },
        { VK_FORMAT_B8G8R8_SINT, "VK_FORMAT_B8G8R8_SINT" },
        { VK_FORMAT_B8G8R8_SRGB, "VK_FORMAT_B8G8R8_SRGB" },
        { VK_FORMAT_R8G8B8A8_UNORM, "VK_FORMAT_R8G8B8A8_UNORM" },
        { VK_FORMAT_R8G8B8A8_SNORM, "VK_FORMAT_R8G8B8A8_SNORM" },
        { VK_FORMAT_R8G8B8A8_USCALED, "VK_FORMAT_R8G8B8A8_USCALED" },
        { VK_FORMAT_R8G8B8A8_SSCALED, "VK_FORMAT_R8G8B8A8_SSCALED" },
        { VK_FORMAT_R8G8B8A8_UINT, "VK_FORMAT_R8G8B8A8_UINT" },
        { VK_FORMAT_R8G8B8A8_SINT, "VK_FORMAT_R8G8B8A8_SINT" },
        { VK_FORMAT_R8G8B8A8_SRGB, "VK_FORMAT_R8G8B8A8_SRGB" },
        { VK_FORMAT_B8G8R8A8_UNORM, "VK_FORMAT_B8G8R8A8_UNORM" },
        { VK_FORMAT_B8G8R8A8_SNORM, "VK_FORMAT_B8G8R8A8_SNORM" },
        { VK_FORMAT_B8G8R8A8_USCALED, "VK_FORMAT_B8G8R8A8_USCALED" },
        { VK_FORMAT_B8G8R8A8_SSCALED, "VK_FORMAT_B8G8R8A8_SSCALED" },
        { VK_FORMAT_B8G8R8A8_UINT, "VK_FORMAT_B8G8R8A8_UINT" },
        { VK_FORMAT_B8G8R8A8_SINT, "VK_FORMAT_B8G8R8A8_SINT" },
        { VK_FORMAT_B8G8R8A8_SRGB, "VK_FORMAT_B8G8R8A8_SRGB" },
        { VK_FORMAT_A8B8G8R8_UNORM_PACK32, "VK_FORMAT_A8B8G8R8_UNORM_PACK32" },
        { VK_FORMAT_A8B8G8R8_SNORM_PACK32, "VK_FORMAT_A8B8G8R8_SNORM_PACK32" },
        { VK_FORMAT_A8B8G8R8_USCALED_PACK32, "VK_FORMAT_A8B8G8R8_USCALED_PACK32" },
        { VK_FORMAT_A8B8G8R8_SSCALED_PACK32, "VK_FORMAT_A8B8G8R8_SSCALED_PACK32" },
        { VK_FORMAT_A8B8G8R8_UINT_PACK32, "VK_FORMAT_A8B8G8R8_UINT_PACK32" },
        { VK_FORMAT_A8B8G8R8_SINT_PACK32, "VK_FORMAT_A8B8G8R8_SINT_PACK32" },
        { VK_FORMAT_A8B8G8R8_SRGB_PACK32, "VK_FORMAT_A8B8G8R8_SRGB_PACK32" },
        { VK_FORMAT_A2R10G10B10_UNORM_PACK32, "VK_FORMAT_A2R10G10B10_UNORM_PACK32" },
        { VK_FORMAT_A2R10G10B10_SNORM_PACK32, "VK_FORMAT_A2R10G10B10_SNORM_PACK32" },
        { VK_FORMAT_A2R10G10B10_USCALED_PACK32, "VK_FORMAT_A2R10G10B10_USCALED_PACK32" },
        { VK_FORMAT_A2R10G10B10_SSCALED_PACK32, "VK_FORMAT_A2R10G10B10_SSCALED_PACK32" },
        { VK_FORMAT_A2R10G10B10_UINT_PACK32, "VK_FORMAT_A2R10G10B10_UINT_PACK32" },
        { VK_FORMAT_A2R10G10B10_SINT_PACK32, "VK_FORMAT_A2R10G10B10_SINT_PACK32" },
        { VK_FORMAT_A2B10G10R10_UNORM_PACK32, "VK_FORMAT_A2B10G10R10_UNORM_PACK32" },
        { VK_FORMAT_A2B10G10R10_SNORM_PACK32, "VK_FORMAT_A2B10G10R10_SNORM_PACK32" },
        { VK_FORMAT_A2B10G10R10_USCALED_PACK32, "VK_FORMAT_A2B10G10R10_USCALED_PACK32" },
        { VK_FORMAT_A2B10G10R10_SSCALED_PACK32, "VK_FORMAT_A2B10G10R10_SSCALED_PACK32" },
        { VK_FORMAT_A2B10G10R10_UINT_PACK32, "VK_FORMAT_A2B10G10R10_UINT_PACK32" },
        { VK_FORMAT_A2B10G10R10_SINT_PACK32, "VK_FORMAT_A2B10G10R10_SINT_PACK32" },
        { VK_FORMAT_R16_UNORM, "VK_FORMAT_R16_UNORM" },
        { VK_FORMAT_R16_SNORM, "VK_FORMAT_R16_SNORM" },
        { VK_FORMAT_R16_USCALED, "VK_FORMAT_R16_USCALED" },
        { VK_FORMAT_R16_SSCALED, "VK_FORMAT_R16_SSCALED" },
        { VK_FORMAT_R16_UINT, "VK_FORMAT_R16_UINT" },
        { VK_FORMAT_R16_SINT, "VK_FORMAT_R16_SINT" },
        { VK_FORMAT_R16_SFLOAT, "VK_FORMAT_R16_SFLOAT" },
        { VK_FORMAT_R16G16_UNORM, "VK_FORMAT_R16G16_UNORM" },
        { VK_FORMAT_R16G16_SNORM, "VK_FORMAT_R16G16_SNORM" },
        { VK_FORMAT_R16G16_USCALED, "VK_FORMAT_R16G16_USCALED" },
        { VK_FORMAT_R16G16_SSCALED, "VK_FORMAT_R16G16_SSCALED" },
        { VK_FORMAT_R16G16_UINT, "VK_FORMAT_R16G16_UINT" },
        { VK_FORMAT_R16G16_SINT, "VK_FORMAT_R16G16_SINT" },
        { VK_FORMAT_R16G16_SFLOAT, "VK_FORMAT_R16G16_SFLOAT" },
        { VK_FORMAT_R16G16B16_UNORM, "VK_FORMAT_R16G16B16_UNORM" },
        { VK_FORMAT_R16G16B16_SNORM, "VK_FORMAT_R16G16B16_SNORM" },
        { VK_FORMAT_R16G16B16_USCALED, "VK_FORMAT_R16G16B16_USCALED" },
        { VK_FORMAT_R16G16B16_SSCALED, "VK_FORMAT_R16G16B16_SSCALED" },
        { VK_FORMAT_R16G16B16_UINT, "VK_FORMAT_R16G16B16_UINT" },
        { VK_FORMAT_R16G16B16_SINT, "VK_FORMAT_R16G16B16_SINT" },
        { VK_FORMAT_R16G16B16_SFLOAT, "VK_FORMAT_R16G16B16_SFLOAT" },
        { VK_FORMAT_R16G16B16A16_UNORM, "VK_FORMAT_R16G16B16A16_UNORM" },
        { VK_FORMAT_R16G16B16A16_SNORM, "VK_FORMAT_R16G16B16A16_SNORM" },
        { VK_FORMAT_R16G16B16A16_USCALED, "VK_FORMAT_R16G16B16A16_USCALED" },
        { VK_FORMAT_R16G16B16A16_SSCALED, "VK_FORMAT_R16G16B16A16_SSCALED" },
        { VK_FORMAT_R16G16B16A16_UINT, "VK_FORMAT_R16G16B16A16_UINT" },
        { VK_FORMAT_R16G16B16A16_SINT, "VK_FORMAT_R16G16B16A16_SINT" },
        { VK_FORMAT_R16G16B16A16_SFLOAT, "VK_FORMAT_R16G16B16A16_SFLOAT" },
        { VK_FORMAT_R32_UINT, "VK_FORMAT_R32_UINT" },
        { VK_FORMAT_R32_SINT, "VK_FORMAT_R32_SINT" },
        { VK_FORMAT_R32_SFLOAT, "VK_FORMAT_R32_SFLOAT" },
        { VK_FORMAT_R32G32_UINT, "VK_FORMAT_R32G32_UINT" },
        { VK_FORMAT_R32G32_SINT, "VK_FORMAT_R32G32_SINT" },
        { VK_FORMAT_R32G32_SFLOAT, "VK_FORMAT_R32G32_SFLOAT" },
        { VK_FORMAT_R32G32B32_UINT, "VK_FORMAT_R32G32B32_UINT" },
        { VK_FORMAT_R32G32B32_SINT, "VK_FORMAT_R32G32B32_SINT" },
        { VK_FORMAT_R32G32B32_SFLOAT, "VK_FORMAT_R32G32B32_SFLOAT" },
        { VK_FORMAT_R32G32B32A32_UINT, "VK_FORMAT_R32G32B32A32_UINT" },
        { VK_FORMAT_R32G32B32A32_SINT, "VK_FORMAT_R32G32B32A32_SINT" },
        { VK_FORMAT_R32G32B32A32_SFLOAT, "VK_FORMAT_R32G32B32A32_SFLOAT" },
        { VK_FORMAT_R64_UINT, "VK_FORMAT_R64_UINT" },
        { VK_FORMAT_R64_SINT, "VK_FORMAT_R64_SINT" },
        { VK_FORMAT_R64_SFLOAT, "VK_FORMAT_R64_SFLOAT" },
        { VK_FORMAT_R64G64_UINT, "VK_FORMAT_R64G64_UINT" },
        { VK_FORMAT_R64G64_SINT, "VK_FORMAT_R64G64_SINT" },
        { VK_FORMAT_R64G64_SFLOAT, "VK_FORMAT_R64G64_SFLOAT" },
        { VK_FORMAT_R64G64B64_UINT, "VK_FORMAT_R64G64B64_UINT" },
        { VK_FORMAT_R64G64B64_SINT, "VK_FORMAT_R64G64B64_SINT" },
        { VK_FORMAT_R64G64B64_SFLOAT, "VK_FORMAT_R64G64B64_SFLOAT" },
        { VK_FORMAT_R64G64B64A64_UINT, "VK_FORMAT_R64G64B64A64_UINT" },
        { VK_FORMAT_R64G64B64A64_SINT, "VK_FORMAT_R64G64B64A64_SINT" },
        { VK_FORMAT_R64G64B64A64_SFLOAT, "VK_FORMAT_R64G64B64A64_SFLOAT" },
        { VK_FORMAT_B10G11R11_UFLOAT_PACK32, "VK_FORMAT_B10G11R11_UFLOAT_PACK32" },
        { VK_FORMAT_E5B9G9R9_UFLOAT_PACK32, "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32" },
        { VK_FORMAT_D16_UNORM, "VK_FORMAT_D16_UNORM" },
        { VK_FORMAT_X8_D24_UNORM_PACK32, "VK_FORMAT_X8_D24_UNORM_PACK32" },
        { VK_FORMAT_D32_SFLOAT, "VK_FORMAT_D32_SFLOAT" },
        { VK_FORMAT_S8_UINT, "VK_FORMAT_S8_UINT" },
        { VK_FORMAT_D16_UNORM_S8_UINT, "VK_FORMAT_D16_UNORM_S8_UINT" },
        { VK_FORMAT_D24_UNORM_S8_UINT, "VK_FORMAT_D24_UNORM_S8_UINT" },
        { VK_FORMAT_D32_SFLOAT_S8_UINT, "VK_FORMAT_D32_SFLOAT_S8_UINT" },
        { VK_FORMAT_BC1_RGB_UNORM_BLOCK, "VK_FORMAT_BC1_RGB_UNORM_BLOCK" },
        { VK_FORMAT_BC1_RGB_SRGB_BLOCK, "VK_FORMAT_BC1_RGB_SRGB_BLOCK" },
        { VK_FORMAT_BC1_RGBA_UNORM_BLOCK, "VK_FORMAT_BC1_RGBA_UNORM_BLOCK" },
        { VK_FORMAT_BC1_RGBA_SRGB_BLOCK, "VK_FORMAT_BC1_RGBA_SRGB_BLOCK" },
        { VK_FORMAT_BC2_UNORM_BLOCK, "VK_FORMAT_BC2_UNORM_BLOCK" },
        { VK_FORMAT_BC2_SRGB_BLOCK, "VK_FORMAT_BC2_SRGB_BLOCK" },
        { VK_FORMAT_BC3_UNORM_BLOCK, "VK_FORMAT_BC3_UNORM_BLOCK" },
        { VK_FORMAT_BC3_SRGB_BLOCK, "VK_FORMAT_BC3_SRGB_BLOCK" },
        { VK_FORMAT_BC4_UNORM_BLOCK, "VK_FORMAT_BC4_UNORM_BLOCK" },
        { VK_FORMAT_BC4_SNORM_BLOCK, "VK_FORMAT_BC4_SNORM_BLOCK" },
        { VK_FORMAT_BC5_UNORM_BLOCK, "VK_FORMAT_BC5_UNORM_BLOCK" },
        { VK_FORMAT_BC5_SNORM_BLOCK, "VK_FORMAT_BC5_SNORM_BLOCK" },
        { VK_FORMAT_BC6H_UFLOAT_BLOCK, "VK_FORMAT_BC6H_UFLOAT_BLOCK" },
        { VK_FORMAT_BC6H_SFLOAT_BLOCK, "VK_FORMAT_BC6H_SFLOAT_BLOCK" },
        { VK_FORMAT_BC7_UNORM_BLOCK, "VK_FORMAT_BC7_UNORM_BLOCK" },
        { VK_FORMAT_BC7_SRGB_BLOCK, "VK_FORMAT_BC7_SRGB_BLOCK" },
        { VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK, "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK" },
        { VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK, "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK" },
        { VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK, "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK" },
        { VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK, "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK" },
        { VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK, "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK" },
        { VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK, "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK" },
        { VK_FORMAT_EAC_R11_UNORM_BLOCK, "VK_FORMAT_EAC_R11_UNORM_BLOCK" },
        { VK_FORMAT_EAC_R11_SNORM_BLOCK, "VK_FORMAT_EAC_R11_SNORM_BLOCK" },
        { VK_FORMAT_EAC_R11G11_UNORM_BLOCK, "VK_FORMAT_EAC_R11G11_UNORM_BLOCK" },
        { VK_FORMAT_EAC_R11G11_SNORM_BLOCK, "VK_FORMAT_EAC_R11G11_SNORM_BLOCK" },
        { VK_FORMAT_ASTC_4x4_UNORM_BLOCK, "VK_FORMAT_ASTC_4x4_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_4x4_SRGB_BLOCK, "VK_FORMAT_ASTC_4x4_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_5x4_UNORM_BLOCK, "VK_FORMAT_ASTC_5x4_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_5x4_SRGB_BLOCK, "VK_FORMAT_ASTC_5x4_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_5x5_UNORM_BLOCK, "VK_FORMAT_ASTC_5x5_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_5x5_SRGB_BLOCK, "VK_FORMAT_ASTC_5x5_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_6x5_UNORM_BLOCK, "VK_FORMAT_ASTC_6x5_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_6x5_SRGB_BLOCK, "VK_FORMAT_ASTC_6x5_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_6x6_UNORM_BLOCK, "VK_FORMAT_ASTC_6x6_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_6x6_SRGB_BLOCK, "VK_FORMAT_ASTC_6x6_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_8x5_UNORM_BLOCK, "VK_FORMAT_ASTC_8x5_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_8x5_SRGB_BLOCK, "VK_FORMAT_ASTC_8x5_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_8x6_UNORM_BLOCK, "VK_FORMAT_ASTC_8x6_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_8x6_SRGB_BLOCK, "VK_FORMAT_ASTC_8x6_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_8x8_UNORM_BLOCK, "VK_FORMAT_ASTC_8x8_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_8x8_SRGB_BLOCK, "VK_FORMAT_ASTC_8x8_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_10x5_UNORM_BLOCK, "VK_FORMAT_ASTC_10x5_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_10x5_SRGB_BLOCK, "VK_FORMAT_ASTC_10x5_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_10x6_UNORM_BLOCK, "VK_FORMAT_ASTC_10x6_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_10x6_SRGB_BLOCK, "VK_FORMAT_ASTC_10x6_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_10x8_UNORM_BLOCK, "VK_FORMAT_ASTC_10x8_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_10x8_SRGB_BLOCK, "VK_FORMAT_ASTC_10x8_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_10x10_UNORM_BLOCK, "VK_FORMAT_ASTC_10x10_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_10x10_SRGB_BLOCK, "VK_FORMAT_ASTC_10x10_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_12x10_UNORM_BLOCK, "VK_FORMAT_ASTC_12x10_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_12x10_SRGB_BLOCK, "VK_FORMAT_ASTC_12x10_SRGB_BLOCK" },
        { VK_FORMAT_ASTC_12x12_UNORM_BLOCK, "VK_FORMAT_ASTC_12x12_UNORM_BLOCK" },
        { VK_FORMAT_ASTC_12x12_SRGB_BLOCK, "VK_FORMAT_ASTC_12x12_SRGB_BLOCK" },
        { VK_FORMAT_G8B8G8R8_422_UNORM, "VK_FORMAT_G8B8G8R8_422_UNORM" },
        { VK_FORMAT_B8G8R8G8_422_UNORM, "VK_FORMAT_B8G8R8G8_422_UNORM" },
        { VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM, "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM" },
        { VK_FORMAT_G8_B8R8_2PLANE_420_UNORM, "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM" },
        { VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM, "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM" },
        { VK_FORMAT_G8_B8R8_2PLANE_422_UNORM, "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM" },
        { VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM, "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM" },
        { VK_FORMAT_R10X6_UNORM_PACK16, "VK_FORMAT_R10X6_UNORM_PACK16" },
        { VK_FORMAT_R10X6G10X6_UNORM_2PACK16, "VK_FORMAT_R10X6G10X6_UNORM_2PACK16" },
        { VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16, "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16" },
        { VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16, "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16" },
        { VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16, "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16" },
        { VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16, "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16" },
        { VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16, "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16" },
        { VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16, "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16" },
        { VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16, "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16" },
        { VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16, "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16" },
        { VK_FORMAT_R12X4_UNORM_PACK16, "VK_FORMAT_R12X4_UNORM_PACK16" },
        { VK_FORMAT_R12X4G12X4_UNORM_2PACK16, "VK_FORMAT_R12X4G12X4_UNORM_2PACK16" },
        { VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16, "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16" },
        { VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16, "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16" },
        { VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16, "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16" },
        { VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16, "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16" },
        { VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16, "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16" },
        { VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16, "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16" },
        { VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16, "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16" },
        { VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16, "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16" },
        { VK_FORMAT_G16B16G16R16_422_UNORM, "VK_FORMAT_G16B16G16R16_422_UNORM" },
        { VK_FORMAT_B16G16R16G16_422_UNORM, "VK_FORMAT_B16G16R16G16_422_UNORM" },
        { VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM, "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM" },
        { VK_FORMAT_G16_B16R16_2PLANE_420_UNORM, "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM" },
        { VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM, "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM" },
        { VK_FORMAT_G16_B16R16_2PLANE_422_UNORM, "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM" },
        { VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM, "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM" },
        { VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG, "VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG" },
        { VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG, "VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG" },
        { VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG, "VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG" },
        { VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG, "VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG" },
        { VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG, "VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG" },
        { VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG, "VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG" },
        { VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG, "VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG" },
        { VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG, "VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG" },
        { VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT" },
        { VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT, "VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT" }
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

    std::string_view vulkan_utils::to_string(VkFormat format) {
        auto it = format_names.find(format);
        if (it == format_names.end()) return invalid_name;
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

        assert(dev);

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

        assert(dev);

        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);
        std::vector<VkQueueFamilyProperties> props(count);
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, props.data());
        return props;
    }

    result<std::vector<VkPhysicalDevice>, vulkan_error>
    vulkan_utils::query_physical_devices(VkInstance inst) {

        assert(inst);

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

        assert(dev);
        assert(surface);

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

        assert(dev);
        assert(surface);

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

        assert(dev);
        assert(surface);

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

        assert(dev);

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

    result<std::vector<VkImage>, vulkan_error> vulkan_utils::query_swapchain_images(VkDevice dev, VkSwapchainKHR swapchain) {

        assert(dev);
        assert(swapchain);

        uint32_t count = 0;
        auto res = vkGetSwapchainImagesKHR(dev, swapchain, &count, nullptr);
        if (res != VkResult::VK_SUCCESS) {
            return vulkan_error("Failed to query swap chain images", res);
        }
        std::vector<VkImage> images(count);
        res = vkGetSwapchainImagesKHR(dev, swapchain, &count, images.data());
        if (res != VkResult::VK_SUCCESS) {
            return vulkan_error("Failed to query swap chain images", res);
        }

        return images;
    }

    VkComponentMapping vulkan_utils::make_identity_component_mapping() noexcept {
        return VkComponentMapping {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        };
    }
}
