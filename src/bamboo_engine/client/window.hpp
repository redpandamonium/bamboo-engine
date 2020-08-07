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

#ifndef BAMBOOENGINE_WINDOW_HPP
#define BAMBOOENGINE_WINDOW_HPP

#include <string>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "../util/macros.hpp"

namespace bbge {

    /**
     * @brief Interface for native windows to implement.
     */
    struct window {

        static constexpr const glm::ivec2 position_dontcare = { -1, -1 };
        static constexpr const glm::ivec2 position_center = { -2, -2 };

        virtual ~window() = default;
    };

    class glfw_window final : public window {
    public:

        glfw_window(std::string&& title, glm::ivec2 position, glm::ivec2 dimensions);
        ~glfw_window() override;

        BBGE_NO_COPIES(glfw_window);

        [[nodiscard]] GLFWwindow* get_handle() const noexcept;

    private:

        GLFWwindow* m_handle;
        std::string m_title;
    };
}


#endif //BAMBOOENGINE_WINDOW_HPP
