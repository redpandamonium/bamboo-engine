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
// Created by Leon Suchy on 06.08.20.
//

#ifndef BAMBOOENGINE_GLFW_HPP
#define BAMBOOENGINE_GLFW_HPP

#include "../util/version.hpp"
#include "../util/macros.hpp"
#include <stdexcept>

namespace bbge {

    /**
     * @brief Exception type for GLFW.
     */
    struct glfw_error : public std::runtime_error {
        explicit glfw_error(const std::string& msg);
    };

    /**
     * @brief GLFW API wrapper.
     */
    class glfw_api {
    public:

        glfw_api();

        ~glfw_api();

        BBGE_NO_COPIES(glfw_api);
        BBGE_NO_MOVES(glfw_api);

        /**
         * @brief Get the GLFW library version.
         * @return Version
         */
        [[nodiscard]] static version get_version() noexcept;

        /**
         * @brief Callback function that gets called by GLFW when errors occur.
         * @param err_code A GLFW error code (see https://www.glfw.org/docs/latest/group__errors.html)
         * @param msg A human-readable description of the error
         * @note DO NOT CALL THIS!
         */
        static void error_callback(int err_code, const char* msg);

    private:

    };
}

#endif //BAMBOOENGINE_GLFW_HPP
