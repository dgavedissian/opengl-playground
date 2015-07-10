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
    src/framework/Framebuffer.cpp
    src/framework/Framework.cpp
    src/framework/Mesh.cpp
    src/framework/Shader.cpp
    src/framework/Texture.cpp
    src/framework/Utils.cpp)

set(HEADER_FILES
    src/framework/Common.h
    src/framework/Framebuffer.h
    src/framework/Framework.h
    src/framework/Mesh.h
    src/framework/Shader.h
    src/framework/Texture.h
    src/framework/Utils.h)

# Main
set(SOURCE_FILES ${SOURCE_FILES} src/Main.cpp)
set(HEADER_FILES ${HEADER_FILES})

# Executable
include_directories(${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} ${FREEIMAGE_INCLUDE_DIRS})
add_executable(Deferred ${SOURCE_FILES} ${HEADER_FILES} WIN32)
target_link_libraries(Deferred ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} ${FREEIMAGE_LIBRARIES})
create_vcproj_userfile(Deferred)
mirror_physical_directories(${SOURCE_FILES} ${HEADER_FILES})
if(NOT APPLE)
    target_link_libraries(Deferred ${GLEW_LIBRARY})
endif()

