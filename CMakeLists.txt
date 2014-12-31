cmake_minimum_required(VERSION 3.0)
project(DeferredShading)

# Module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# Source files
set(SOURCE_FILES
  src/main.cpp)
set(HEADER_FILES)

# Dependencies
find_package(SDL2 REQUIRED)

# Executable
include_directories(${SDL2_INCLUDE_DIR})
add_executable(deferred ${SOURCE_FILES})
target_link_libraries(deferred ${SDL2_LIBRARY})

