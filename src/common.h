/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef COMMON_H
#define COMMON_H

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>

// OS X doesn't require glew: http://stackoverflow.com/a/11213354
#ifdef __APPLE__
#   include <OpenGL/gl3.h>
#else
#   include <GL/glew.h>
#endif

#include <SDL.h>

#include "glm/glm.hpp"

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::make_shared;
using std::shared_ptr;

#endif /* COMMON_H */
