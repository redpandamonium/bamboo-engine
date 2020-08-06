#include <iostream>
#include <chrono>
#include <thread>
#include <bamboo_engine/client/sdl.hpp>
#include <bamboo_engine/client/window.hpp>
#include <bamboo_engine/util/logging.hpp>

int main(int argc, char** argv) {

    using namespace bbge;
    using namespace std::string_literals;

    logging logging_system { };
    sdl_api sdl { };
    std::unique_ptr<window> win = std::make_unique<sdl_window>(
        "Bamboo Engine"s, glm::ivec2 { 1024, 720 }, window::position_center);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}
