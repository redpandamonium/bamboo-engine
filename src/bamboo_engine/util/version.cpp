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

#include "version.hpp"
#include <fmt/format.h>
#include <boost/algorithm/string/split.hpp>

namespace bbge {

    std::string version::string() const {
        return m_patch >= 0 ?
            fmt::format("{}.{}.{}", m_major, m_minor, m_patch) :
            fmt::format("{}.{}", m_major, m_minor);
    }

    int version::get_major() const noexcept {
        return m_major;
    }

    int version::get_minor() const noexcept {
        return m_minor;
    }

    int version::get_patch() const noexcept {
        return m_patch;
    }

    bool version::operator==(const version& rhs) const noexcept {
        return m_major == rhs.m_major &&
               m_minor == rhs.m_minor &&
               m_patch == rhs.m_patch;
    }

    bool version::operator!=(const version& rhs) const noexcept {
        return !(rhs == *this);
    }

    bool version::operator<(const version& rhs) const noexcept {
        if (m_major > rhs.m_major) return false;
        if (m_minor > rhs.m_minor) return false;
        if (m_patch == not_present || rhs.m_patch == not_present) return false;
        return m_patch < rhs.m_patch;
    }

    version version::from_string(const std::string_view& str) {

        // empty string
        if (str.empty()) {
            return version { };
        }

        // split string into version tokens
        std::vector<std::string> tokens;
        tokens.reserve(3);
        boost::algorithm::split(tokens, str, [](char c) { return c == '.'; });

        // version scheme too complicated
        if (tokens.size() > 3) {
            throw std::invalid_argument("Version string contains too many components.");
        }

        // parse components
        int major = std::stoi(tokens[0]);
        int minor = tokens.size() > 1 ? std::stoi(tokens[1]) : 0;
        int patch = tokens.size() > 2 ? std::stoi(tokens[2]) : not_present;

        // build version
        return version { major, minor, patch };
    }

    bool version::has_patch() const noexcept {
        return m_patch != not_present;
    }

    bool version::has_minor() const noexcept {
        return m_minor != not_present;
    }
}
