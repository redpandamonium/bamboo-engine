//
// Created by blaze on 8/12/20.
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
