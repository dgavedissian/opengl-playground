cmake_minimum_required(VERSION 3.0)
project(GLPrototypes)

# Module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# Utilities
include(Utilities)
enable_cpp11() 
enable_maximum_warnings()

# Dependencies
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

# Framework
set(CPP_FILES
    src/framework/gl3w.c
    src/framework/Framebuffer.cpp
    src/framework/Framework.cpp
    src/framework/Mesh.cpp
    src/framework/Shader.cpp
    src/framework/Texture.cpp
    src/framework/Utils.cpp)
set(H_FILES
    src/framework/Common.h
    src/framework/Framebuffer.h
    src/framework/Framework.h
    src/framework/Mesh.h
    src/framework/Shader.h
    src/framework/Texture.h
    src/framework/Utils.h)
set(FRAMEWORK_FILES ${CPP_FILES} ${H_FILES})

# Framework include dirs
include_directories(src/framework)
include_directories(${OPENGL_INCLUDE_DIR} ${SDL2_INCLUDE_DIR})

# Add a new prototype
macro(add_prototype NAME)
    add_executable(${NAME} ${FRAMEWORK_FILES} ${SOURCE_FILES})
    target_link_libraries(${NAME} ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})
    create_vcproj_userfile(${NAME})
    mirror_physical_directories(${FRAMEWORK_FILES} ${SOURCE_FILES})
endmacro()

# Deferred Shading
set(SOURCE_FILES src/Main.cpp)
add_prototype(DeferredShading)

# PBS
## TODO
