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

#ifndef BAMBOOENGINE_LOGGING_HPP
#define BAMBOOENGINE_LOGGING_HPP

#ifndef NDEBUG
    // Debug builds can log trace
    #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
    // No trace logging in release
    #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif
#include <spdlog/spdlog.h>

namespace bbge {

    /**
     * @brief Helper class to set up logging.
     */
    struct logging {

        logging();
        ~logging();
    };

}

#endif //BAMBOOENGINE_LOGGING_HPP
