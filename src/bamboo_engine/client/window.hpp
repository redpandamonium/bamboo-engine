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
#include <SDL2/SDL_video.h>

namespace bbge {

    /**
     * @brief Interface for native windows to implement.
     */
    struct window {

        static constexpr const glm::ivec2 position_dontcare = { -1, -1 };
        static constexpr const glm::ivec2 position_center = { -2, -2 };

        virtual ~window() = default;
    };

    /**
     * @brief Window implementation using the SDL2 API.
     */
    class sdl_window final : public window {
    public:

        /**
         * @brief Create a new window.
         * @param name Window title
         * @param dimensions Dimensions of the window
         * @param position Position of the window on the main monitor
         * @see bbge::window for position constants
         *
         * SDL must be init before this.
         */
        sdl_window(const std::string& name, glm::ivec2 dimensions, glm::ivec2 position);

        /**
         * @brief Destructor. Closes the window.
         */
        ~sdl_window() override;

        /**
         * @brief Get the internal handle of this window.
         * @return Internal handle.
         */
        [[nodiscard]]
        SDL_Window* get_handle() noexcept;

    private:

        SDL_Window* m_handle;

        [[nodiscard]]
        static glm::ivec2 convert_position(glm::ivec2 pos) noexcept;
    };
}


#endif //BAMBOOENGINE_WINDOW_HPP
