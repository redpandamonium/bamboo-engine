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

#include "sdl.hpp"
#include "../util/logging.hpp"

namespace bbge {

    sdl_error::sdl_error(const std::string& msg) : runtime_error(msg + " " + SDL_GetError()) {

    }

    sdl_api::sdl_api() {
        auto res = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER |
                            SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK);
        if (res != 0) {
            throw sdl_error("Failed to init the SDL API.");
        }

        SPDLOG_DEBUG("Initialized SDL API.");
    }

    sdl_api::~sdl_api() {
        SPDLOG_DEBUG("Shutting down SDL API.");
        SDL_Quit();
    }
}