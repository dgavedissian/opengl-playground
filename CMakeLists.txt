cmake_minimum_required(VERSION 3.0)
project(DeferredShading)

# Module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# Utilities
include(Utilities)
enable_cpp11() 
enable_maximum_warnings()

# Dependencies
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(FreeImage REQUIRED)
if(NOT APPLE)
    find_package(GLEW REQUIRED)
endif()

# Framework
set(SOURCE_FILES
    src/framework/framebuffer.cpp
    src/framework/framework.cpp
    src/framework/shader.cpp
    src/framework/texture.cpp
    src/framework/utils.cpp
    src/framework/vertexbuffer.cpp)

set(HEADER_FILES
    src/framework/common.h
    src/framework/framebuffer.h
    src/framework/framework.h
    src/framework/shader.h
    src/framework/texture.h
    src/framework/utils.h
    src/framework/vertexbuffer.h)

# Main
set(SOURCE_FILES ${SOURCE_FILES} src/main.cpp)
set(HEADER_FILES ${HEADER_FILES})

# Executable
include_directories(${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} ${FREEIMAGE_INCLUDE_DIRS})
add_executable(deferred ${SOURCE_FILES} ${HEADER_FILES} WIN32)
target_link_libraries(deferred ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} ${FREEIMAGE_LIBRARIES})
create_vcproj_userfile(deferred)
mirror_physical_directories(${SOURCE_FILES} ${HEADER_FILES})
if(NOT APPLE)
    target_link_libraries(deferred ${GLEW_LIBRARY})
endif()

