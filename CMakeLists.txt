cmake_minimum_required(VERSION 3.10)
project(LightSimulation)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Explicitly specify module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/share/eigen3/cmake")

find_package(Eigen3 3.4 REQUIRED)

# Include directories
include_directories(include)

# Source files for the library
set(SOURCES
    src/LightSimulation.cpp
)

# Create static library
add_library(LightSimulationLib STATIC ${SOURCES})

# Create executable
add_executable(LightSimulation main.cpp)

# Link library to executable
target_link_libraries(LightSimulation LightSimulationLib Eigen3::Eigen)