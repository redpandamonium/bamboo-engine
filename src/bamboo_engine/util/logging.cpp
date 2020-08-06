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

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>
#include "logging.hpp"

namespace bbge {


    logging::logging() {

        // Thread pool for async logging
        spdlog::init_thread_pool(1024, 2);

        // Sinks
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/latest.log");
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        std::vector<spdlog::sink_ptr> sinks { console_sink, file_sink };

        // Create logger
        auto logger = std::make_shared<spdlog::async_logger>(
            "aevc", sinks.begin(), sinks.end(),
            spdlog::thread_pool(), spdlog::async_overflow_policy::block
        );
        spdlog::set_default_logger(logger);

        // Set level based on if this is a debug build or not
        #ifndef NDEBUG
            file_sink->set_level(spdlog::level::trace);
            console_sink->set_level(spdlog::level::trace);
            spdlog::set_level(spdlog::level::trace);
            logger->set_level(spdlog::level::trace);
        #else
            file_sink->set_level(spdlog::level::debug);
            console_sink->set_level(spdlog::level::info);
            spdlog::set_level(spdlog::level::debug);
            logger->set_level(spdlog::level::debug);
        #endif

        // Set the log patterns
        file_sink->set_pattern("[%Y-%m-%d %T.%e] [t:%t] [%@] [%l] %v");
        spdlog::set_pattern("[%Y-%m-%d %T.%e] [t:%t] [%l] %v");

        // Flush policies
        spdlog::flush_every(std::chrono::seconds(2));
        logger->flush_on(spdlog::level::err);
    }

    logging::~logging() {
        // Make sure everything is flushed and closed.
        spdlog::shutdown();
    }
}