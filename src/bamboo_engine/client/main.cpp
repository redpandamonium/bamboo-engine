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

#include <iostream>
#include <chrono>
#include <thread>
#include <bamboo_engine/client/window.hpp>
#include <bamboo_engine/util/logging.hpp>
#include <bamboo_engine/graphics/vulkan.hpp>
#include "glfw.hpp"

void print_gpl_notice() {
    auto msg =
        R"(
Bamboo Game Engine  Copyright (C) 2020  Leon Suchy
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
This program comes with a LICENSE file containing the details.
If not provided see <https://www.gnu.org/licenses/> for that purpose.
)";
    std::cout << msg << std::endl;
}

int main(int argc, char** argv) {

    using namespace bbge;
    using namespace std::string_literals;

    logging logging_system { };

    print_gpl_notice();

    try {
        glfw_api glfw { };
        std::unique_ptr<window> win = std::make_unique<glfw_window>(
            "Bamboo Engine"s, window::position_center, glm::ivec2 { 1024, 720 }
        );

        vulkan_instance         vk_instance("Test", version { 0, 1, 1 });
        vulkan_debug_messenger  vk_debug(vk_instance.get_handle());
        vulkan_surface          vk_surface(vk_instance.get_handle(), dynamic_cast<glfw_window&>(*win));
        vulkan_device           vk_device(vk_instance.get_handle(), vk_surface.get_handle());
        vulkan_swap_chain       vk_swapchain(
            vk_instance.get_handle(),
            vk_device.get_physical_device(), vk_device.get_handle(),
            vk_surface.get_handle(), dynamic_cast<glfw_window&>(*win),
            vk_device.get_queue_family_indices()
        );

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    catch (const std::exception& ex) {
        SPDLOG_CRITICAL("EXCEPTION: {}", ex.what());
    }
    catch (...) {
        SPDLOG_CRITICAL("Unknown exception thrown in client. Please inherit from std::exception.");
    }

    return 0;
}
