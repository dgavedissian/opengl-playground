cmake_minimum_required(VERSION 3.0)
project(DeferredShading)

# Module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# Enable C++11
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

    # OS X uses the libc++ standard library as libstdc++ is massively outdated
    if(APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif()

# Compile with many more warnings
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused -Wuninitialized -Wfloat-equal -Woverloaded-virtual")
endif()

# Dependencies
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(FreeImage REQUIRED)
if(NOT APPLE)
    find_package(GLEW REQUIRED)
endif()

# Framework
set(SOURCE_FILES
    src/framebuffer.cpp
    src/framework.cpp
    src/mesh.cpp
    src/shader.cpp
    src/texture.cpp
    src/utils.cpp)

set(HEADER_FILES
    src/common.h
    src/framebuffer.h
    src/framework.h
    src/mesh.h
    src/shader.h
    src/texture.h
    src/utils.h)

# Main
set(SOURCE_FILES ${SOURCE_FILES} src/main.cpp)
set(HEADER_FILES ${HEADER_FILES} src/main.h)

# Executable
include_directories(${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} ${FREEIMAGE_INCLUDE_DIRS})
add_executable(deferred ${SOURCE_FILES})
target_link_libraries(deferred ${OPENGL_LIBRARIES} ${SDL2_LIBRARY} ${FREEIMAGE_LIBRARIES})
if(NOT APPLE)
    target_link_libraries(deferred ${GLEW_LIBRARY})
endif()

