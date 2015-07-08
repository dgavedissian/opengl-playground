/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef COMMON_H
#define COMMON_H

#ifdef WIN32
#	define GLEW_STATIC
#	pragma warning(push, 0)
#endif

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

// OS X doesn't require glew: http://stackoverflow.com/a/11213354
#ifdef __APPLE__
#   include <OpenGL/gl3.h>
#else
#   include <GL/glew.h>
#endif

#include <SDL.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using std::cout;
using std::cerr;
using std::endl;

using std::ifstream;
using std::stringstream;
using std::string;

using std::pair;
using std::make_pair;

using std::unique_ptr;
using std::vector;

typedef uint32_t uint;

#ifdef WIN32
#	pragma warning(pop)
#endif

#endif /* COMMON_H */
