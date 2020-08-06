cmake_minimum_required(VERSION 3.17)
project(BambooEngine VERSION 0.1)

# Project definitions configure
# ----------------------------------------------------------------------------------------------------------------------
configure_file(src/bamboo_engine/cmake_config.hpp.in cmake_config.hpp)

# Build options
# ----------------------------------------------------------------------------------------------------------------------

# Ensure we get C++17 with no compiler extensions enabled
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic -Wno-unknown-pragmas)
endif()

# Dependencies
# ----------------------------------------------------------------------------------------------------------------------
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# Vulkan is not supplied by Conan
find_package(Vulkan REQUIRED)

# Targets
# ----------------------------------------------------------------------------------------------------------------------

add_library(bamboo-engine src/bamboo_engine/client/window.cpp src/bamboo_engine/client/window.hpp
        src/bamboo_engine/client/window.cpp src/bamboo_engine/client/window.hpp
        src/bamboo_engine/util/logging.cpp src/bamboo_engine/util/logging.hpp
        src/bamboo_engine/graphics/vulkan.cpp src/bamboo_engine/graphics/vulkan.hpp
        src/bamboo_engine/client/glfw.cpp src/bamboo_engine/util/version.cpp src/bamboo_engine/util/macros.hpp)
target_include_directories(bamboo-engine INTERFACE src PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(bamboo-engine PUBLIC
        CONAN_PKG::spdlog CONAN_PKG::nlohmann_json CONAN_PKG::gsl-lite CONAN_PKG::boost CONAN_PKG::tsl-robin-map
        CONAN_PKG::glm CONAN_PKG::entt CONAN_PKG::fmt CONAN_PKG::glfw Vulkan::Vulkan)

add_executable(bamboo-engine-client src/bamboo_engine/client/main.cpp)
target_link_libraries(bamboo-engine-client PUBLIC bamboo-engine)


# Tests
# ----------------------------------------------------------------------------------------------------------------------
enable_testing()

add_executable(
        bamboo-engine-test
        test/version_test.cpp)
target_link_libraries(bamboo-engine-test PUBLIC CONAN_PKG::gtest bamboo-engine)
add_test(bamboo-engine-test COMMAND bamboo-engine-test)