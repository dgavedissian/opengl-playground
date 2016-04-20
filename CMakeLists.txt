cmake_minimum_required(VERSION 3.0)
project(Prototypes)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# Utilities
include(Utilities)
enable_cpp11()
enable_maximum_warnings()
set_output_dir("${CMAKE_SOURCE_DIR}/bin")

# Dependencies
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
set(LIBS ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})
include_directories(src src/framework/external ${OPENGL_INCLUDE_DIR} ${SDL2_INCLUDE_DIR})

# Framework
set(EXTERNAL_CPP_FILES
    src/framework/external/gl3w.c)
set(CPP_FILES
    src/framework/Application.cpp
    src/framework/CameraMan.cpp
    src/framework/Framebuffer.cpp
    src/framework/Mesh.cpp
    src/framework/Shader.cpp
    src/framework/Texture.cpp
    src/framework/Utils.cpp)
set(H_FILES
    src/framework/Application.h
    src/framework/CameraMan.h
    src/framework/Common.h
    src/framework/Framebuffer.h
    src/framework/Mesh.h
    src/framework/Shader.h
    src/framework/Texture.h
    src/framework/Utils.h)
set(FRAMEWORK_FILES ${EXTERNAL_CPP_FILES} ${CPP_FILES} ${H_FILES})

add_library(Framework STATIC ${FRAMEWORK_FILES})
target_link_libraries(Framework ${LIBS})
mirror_physical_directories(Framework ${FRAMEWORK_FILES})
set_property(TARGET Framework PROPERTY FOLDER "Framework")

# On Windows, copy the SDL2 DLL to the bin folder
if(WIN32)
    add_custom_command(TARGET Framework POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E copy ${SDL2_INCLUDE_DIR}/../lib/x86/SDL2.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

# Macro for adding a new prototype
macro(add_prototype NAME)
    add_executable(${NAME} ${TYPE} ${SRC_FILES})
    target_link_libraries(${NAME} Framework ${LIBS})
    create_vcproj_userfile(${NAME})
    mirror_physical_directories(${FRAMEWORK_FILES} ${SRC_FILES})
    set_property(TARGET ${NAME} PROPERTY FOLDER "Prototypes")
endmacro()

# Deferred Shading
set(SRC_FILES src/deferred/Main.cpp)
add_prototype(DeferredShading)

# PBS
## TODO

# Galaxy
set(SRC_FILES
    src/galaxy/Main.cpp
    src/galaxy/Galaxy.cpp
    src/galaxy/Galaxy.h)
add_prototype(Galaxy)

# Planet
## TODO
