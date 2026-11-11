cmake_minimum_required(VERSION 3.14)
project(AutonomousCar LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/4.5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

# Include header directories
include_directories(src ml)

# -------------------------------------------------------------
# Target 1: The Main Raylib GUI Simulation
# -------------------------------------------------------------
set(MAIN_SOURCES
    src/main.cpp
    src/Track.cpp
    src/Car.cpp
)
add_executable(AutonomousCar ${MAIN_SOURCES})
target_link_libraries(AutonomousCar PRIVATE raylib)

# -------------------------------------------------------------
# Target 2: The Parallel AI Test Bench (CLI Application)
# -------------------------------------------------------------
add_executable(TestModels src/test_perceptron.cpp)