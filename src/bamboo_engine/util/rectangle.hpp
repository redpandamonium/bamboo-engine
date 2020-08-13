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
// Created by Leon Suchy on 13.08.20.
//

#ifndef BAMBOOENGINE_RECTANGLE_HPP
#define BAMBOOENGINE_RECTANGLE_HPP

#include <vulkan/vulkan.h>
#include <type_traits>
#include <limits>
#include <string>
#include <fmt/format.h>

namespace bbge {

    template <typename T>
    struct rect_t {
        T x { };
        T y { };
        T width { };
        T height { };
    };

    template <typename T>
    VkRect2D to_vulkan_rect(const rect_t<T>& r) {
        static_assert(std::numeric_limits<T>::is_integer, "Can only convert integer rectangles to VkRect2D.");
        return VkRect2D {
            { r.x, r.y },
            { r.width, r.height }
        };
    }

    template <typename T>
    VkViewport to_vulkan_viewport(const rect_t<T>& r) {
        static_assert(std::is_floating_point_v<T>, "Can only convert floating point rectangles to VkViewport.");

        static constexpr const auto min_depth = 0.0f;
        static constexpr const auto max_depth = 1.0f;

        return VkViewport {
            static_cast<float>(r.x), static_cast<float>(r.y),
            static_cast<float>(r.width), static_cast<float>(r.height),
            min_depth, max_depth
        };
    }

    template <typename T>
    std::string to_string(const rect_t<T>& r) {
        return fmt::format("rect {{ x = {}, y = {}, width = {}, height = {} }}", r.x, r.y, r.width, r.height);
    }

    using irect = rect_t<int32_t>;
    using urect = rect_t<uint32_t>;
    using drect = rect_t<double>;
}

#endif //BAMBOOENGINE_RECTANGLE_HPP
