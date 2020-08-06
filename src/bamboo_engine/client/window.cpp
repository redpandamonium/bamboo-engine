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
#include "sdl.hpp"

namespace bbge {


    SDL_Window* sdl_window::get_handle() noexcept {
        return m_handle;
    }

    sdl_window::sdl_window(const std::string& name, glm::ivec2 dimensions, glm::ivec2 position)
        : m_handle(nullptr) {

        // Convert to API specific constants
        position = convert_position(position);

        // Open
        m_handle = SDL_CreateWindow(name.c_str(), position.x, position.y, dimensions.x, dimensions.y, SDL_WINDOW_VULKAN);
        if (!m_handle) {
            throw sdl_error("Failed to open window.");
        }

        SPDLOG_DEBUG("Opened window '{}' [{}].", name, SDL_GetWindowID(m_handle));
    }


    // Disable signed bitwise check. SDL2 leaks this warning.
    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "hicpp-signed-bitwise"

    glm::ivec2 sdl_window::convert_position(glm::ivec2 pos) noexcept {
        if (pos.x == window::position_dontcare.x) pos.x = SDL_WINDOWPOS_UNDEFINED;
        if (pos.x == window::position_center.x) pos.x = SDL_WINDOWPOS_CENTERED;
        if (pos.y == window::position_dontcare.y) pos.y = SDL_WINDOWPOS_UNDEFINED;
        if (pos.y == window::position_center.y) pos.y = SDL_WINDOWPOS_CENTERED;
        return pos;
    }

    sdl_window::~sdl_window() {

        // If the window is valid, we need to destroy it.
        if (m_handle) {
            auto id = SDL_GetWindowID(m_handle);
            SDL_DestroyWindow(m_handle);
            SPDLOG_DEBUG("Closed window [{}].", id);
        }
    }

#pragma clang diagnostic pop

}
