/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef COMMON_H
#define COMMON_H

#ifdef WIN32
#	pragma warning(push, 0)
#endif

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include <SDL.h>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
