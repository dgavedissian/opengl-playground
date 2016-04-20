# Prototype Framework
# Copyright (c) 2013-2014 David Avedissian (avedissian.david@gmail.com)
cmake_minimum_required(VERSION 2.8)
project(Prototypes)


###############################################
# Setup
###############################################

# Include modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake")

# Set default build type
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

# Set install location
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX})
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_INSTALL_PREFIX})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_INSTALL_PREFIX})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_INSTALL_PREFIX})
endforeach()


###############################################
# Platform specific stuff
###############################################

# Set up RPATH on OS X
if(APPLE)
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
  set(CMAKE_INSTALL_RPATH "@rpath/../Frameworks")
endif()

# Compiler options
if(MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
elseif(UNIX AND NOT APPLE)
  add_definitions(-Wno-deprecated-declarations)
  #add_definitions(-Wall -Wextra -pedantic -Wno-unused -Wuninitialized
  #  -Wmaybe-uninitialized -Wshadow -Wfloat-equal -Wold-style-cast
  #  -Woverloaded-virtual)
endif()

# Enable C++11
if(UNIX AND NOT APPLE)
  add_definitions(-std=c++11)
elseif(APPLE)
  add_definitions(-std=c++11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  if(XCODE)
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  endif()
endif()

# create vcproj.user file for Visual Studio to set debug working directory
function(create_vcproj_userfile TARGETNAME)
  if(MSVC)
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/CMake/VisualStudioUserFile.vcproj.user.in
      ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcproj.user
      @ONLY)
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/CMake/VisualStudioUserFile.vcxproj.user.in
      ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcxproj.user
      @ONLY)
  endif()
endfunction()

# Fix for "error C3859: virtual memory range for PCH exceeded"
if(MSVC)
  string(REGEX REPLACE "/Zm[0-9]+ " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm500" CACHE STRING "" FORCE)
endif()

# Append _d for debug builds
set(CMAKE_DEBUG_POSTFIX "_d")


###############################################
# Dependencies
###############################################

# Ogre
find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)
include_directories(${OGRE_INCLUDE_DIRS} ${OGRE_Overlay_INCLUDE_DIRS} ${OIS_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
set(LIBS ${OGRE_LIBRARIES} ${OGRE_Overlay_LIBRARIES} ${OIS_LIBRARIES})


###############################################
# Prototype Template
###############################################

set(FRAMEWORK_SRCS
  src/PrototypeFramework.cpp)
set(FRAMEWORK_HDRS
  src/PrototypeFramework.h)


###############################################
# Galaxy Prototype
###############################################

set(SRCS
  ${FRAMEWORK_SRCS}
  src/galaxy/Galaxy.cpp)

set(HDRS
  ${FRAMEWORK_HDRS}
  src/galaxy/Galaxy.h)

# Set up the project
add_executable(galaxy WIN32 MACOSX_BUNDLE ${SRCS} ${HDRS})
set_target_properties(galaxy PROPERTIES DEBUG_POSTFIX _d)
target_link_libraries(galaxy ${LIBS})
create_vcproj_userfile(galaxy)


###############################################
# Planet Prototype
###############################################

set(SRCS
  ${FRAMEWORK_SRCS}
  src/planet/Main.cpp
  src/planet/Planet.cpp
  src/planet/RingSystem.cpp
  src/planet/Terrain.cpp
  src/planet/simplexnoise.cpp)

set(HDRS
  ${FRAMEWORK_HDRS}
  src/planet/Main.h
  src/planet/Planet.h
  src/planet/RingSystem.h
  src/planet/Terrain.h
  src/planet/simplexnoise.h)

# Set up the project
add_executable(planet WIN32 MACOSX_BUNDLE ${SRCS} ${HDRS})
set_target_properties(planet PROPERTIES DEBUG_POSTFIX _d)
target_link_libraries(planet ${LIBS})
create_vcproj_userfile(planet)

