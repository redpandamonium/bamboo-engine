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

#ifndef BAMBOOENGINE_CUSTOM_FORMATTERS_HPP
#define BAMBOOENGINE_CUSTOM_FORMATTERS_HPP

#include <filesystem>
#include <fmt/format.h>

template<>
struct fmt::formatter<std::filesystem::path> {

    // 'a' for absolute
    // 'r' for relative
    char display = 'r';

    constexpr auto parse(format_parse_context& ctx) {

        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'a' || *it == 'r')) display = *it++;

        // Check if reached the end of the range:
        if (it != end && *it != '}') throw format_error("invalid format");

        // Return an iterator past the end of the parsed range:
        return it;
    }

    template <typename FormatContext>
    auto format(const std::filesystem::path& p, FormatContext& ctx) {
        return format_to(ctx.out(), display == 'a' ? std::filesystem::absolute(p).string() : p.relative_path().string());
    }
};

#endif //BAMBOOENGINE_CUSTOM_FORMATTERS_HPP
