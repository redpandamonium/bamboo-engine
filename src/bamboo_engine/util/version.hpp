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

#ifndef BAMBOOENGINE_VERSION_HPP
#define BAMBOOENGINE_VERSION_HPP

#include <string>

namespace bbge {

    /**
     * @brief Simple struct to represent semantic version numbers.
     */
    class version {
    private:

        static constexpr const int not_present = -1;

        int m_major;
        int m_minor;
        int m_patch = not_present; // optional

    public:

        constexpr version() noexcept
            : m_major(0), m_minor(0), m_patch(not_present) { }
        constexpr version(int major, int minor, int patch) noexcept
            : m_major(major), m_minor(minor), m_patch(patch) { }
        constexpr version(int major, int minor) noexcept
            : m_major(major), m_minor(minor), m_patch(not_present) { }

        [[nodiscard]] static version from_string(const std::string_view& str);

        [[nodiscard]] std::string string() const;

        [[nodiscard]] int get_major() const noexcept;

        [[nodiscard]] int get_minor() const noexcept;

        [[nodiscard]] int get_patch() const noexcept;

        bool operator==(const version& rhs) const noexcept;

        bool operator!=(const version& rhs) const noexcept;

        bool operator<(const version& rhs) const noexcept;

        [[nodiscard]] bool has_patch() const noexcept;

        [[nodiscard]] bool has_minor() const noexcept;
    };
}

#endif //BAMBOOENGINE_VERSION_HPP
