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

#include "window.hpp"
#include "../util/logging.hpp"
#include "glfw.hpp"
#include <SDL2/SDL_vulkan.h>

namespace bbge {

    glfw_window::glfw_window(std::string&& title, [[maybe_unused]] glm::ivec2 position, glm::ivec2 dimensions)
        : m_handle(nullptr), m_title(std::move(title)) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // for vulkan
        m_handle = glfwCreateWindow(dimensions.x, dimensions.y, title.c_str(), nullptr, nullptr);
        if (!m_handle) {
            throw glfw_error(fmt::format("Failed to open window '{}'.", title));
        }
        SPDLOG_TRACE("Opened window '{}'.", m_title);
    }

    glfw_window::~glfw_window() {
        if (m_handle) {
            glfwDestroyWindow(m_handle);
            SPDLOG_TRACE("Closed window '{}'.", m_title);
        }
    }
}
