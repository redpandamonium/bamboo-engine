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

#include "glfw.hpp"
#include <glfw/glfw3.h>
#include "../util/logging.hpp"

namespace bbge {

    glfw_api::glfw_api() {
        glfwSetErrorCallback(&glfw_api::error_callback);
        if (glfwInit() == GLFW_FALSE) {
            throw glfw_error("Failed to init.");
        }
        SPDLOG_DEBUG("Using GLFW3 version {}.", get_version().string());

        if (glfwVulkanSupported() == GLFW_FALSE) {
            throw glfw_error("Vulkan not supported.");
        }
    }

    glfw_api::~glfw_api() {
        glfwTerminate();
    }

    version glfw_api::get_version() noexcept {
        int major, minor, patch;
        glfwGetVersion(&major, &minor, &patch);
        return version { major, minor, patch };
    }

    void glfw_api::error_callback(int err_code, const char* msg) {
        if (err_code == GLFW_NO_ERROR) return; // ignore non-errors
        SPDLOG_ERROR("[glfw] {} (err={}).", msg, err_code);
    }

    glfw_error::glfw_error(const std::string& s) : runtime_error("[glfw] " + s) {

    }
}
