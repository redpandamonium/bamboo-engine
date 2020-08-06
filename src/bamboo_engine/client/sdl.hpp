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

#ifndef BAMBOOENGINE_SDL_HPP
#define BAMBOOENGINE_SDL_HPP

#include <string>
#include <SDL2/SDL.h>
#include <stdexcept>

namespace bbge {

    struct sdl_error : public std::runtime_error {
        explicit sdl_error(const std::string& msg);
    };

    /**
     * @brief RAII wrapper around the SDL API
     */
    class sdl_api {
    public:

        sdl_api();
        ~sdl_api();

    private:
    };
}

#endif //BAMBOOENGINE_SDL_HPP
