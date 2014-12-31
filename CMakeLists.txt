cmake_minimum_required(VERSION 3.0)
project(DeferredShading)

# Module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# Enable C++11
if(UNIX)
  add_definitions(-std=c++11)

  # OS X uses the libc++ standard library as libstdc++ is massively outdated
  if(APPLE)
    add_definitions(-stdlib=libc++)
  endif()
endif()

# Compile with many more warnings with GCC or Clang
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  add_definitions(-Wall -Wextra -pedantic -Wno-unused -Wuninitialized
    -Wmaybe-uninitialized -Wshadow -Wfloat-equal -Wold-style-cast
    -Woverloaded-virtual)
endif()

# Source files
set(SOURCE_FILES
  src/framework.cpp
  src/main.cpp)
set(HEADER_FILES
  src/common.h
  src/framework.h
  src/main.h)

# Dependencies
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)

# Executable
include_directories(${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${SDL2_INCLUDE_DIR})
add_executable(deferred ${SOURCE_FILES})
target_link_libraries(deferred ${OPENGL_LIBRARIES} ${GLEW_LIBRARY} ${SDL2_LIBRARY})

