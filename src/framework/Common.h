/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#pragma once

#ifdef _WIN32
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
typedef uint64_t uint64;

#ifdef WIN32
#	pragma warning(pop)
#endif

// Undef conflicting macros
// Funny how all of these are from Windows.h
#ifdef INFO
    #undef INFO
#endif
#ifdef WARNING
    #undef WARNING
#endif
#ifdef ERROR
    #undef ERROR
#endif
#ifdef CreateWindow
#   undef CreateWindow
#endif

// Info/Warning/Error
#ifdef _WIN32
    #define __SET_CONSOLE_COLOUR(C) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), C);
    #define COLOUR_INFO __SET_CONSOLE_COLOUR(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
    #define COLOUR_WARNING __SET_CONSOLE_COLOUR(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN)
    #define COLOUR_ERROR __SET_CONSOLE_COLOUR(FOREGROUND_INTENSITY | FOREGROUND_RED)
#else
    #define COLOUR_INFO
    #define CONSOLE_WARNING
    #define CONSOLE_ERROR
#endif

inline std::ostream& __info()
{
    COLOUR_INFO
    return cout << "[info] ";
}

inline std::ostream& __warning()
{
    COLOUR_WARNING
    return cerr << "[warning] ";
}

inline std::ostream& __error()
{
    COLOUR_ERROR
    return cerr << "[error] ";
}

#define INFO    __info()
#define WARNING __warning()
#define ERROR   __error()

// Check for GL errors on a statement
#ifdef _DEBUG
    #define CHECK(X) __CHECK(X, __FILE__, __LINE__)
    #define __CHECK(X, FILE, LINE) \
        X \
        { \
            GLuint err = glGetError(); \
            if (err != 0) \
            { \
                ERROR << "glGetError() returned " << err << " (0x" << std::hex << err << ")" << endl; \
                ERROR << "Function: " << #X << " in " << FILE << ":" << LINE << endl; \
                exit(EXIT_FAILURE); \
            } \
        }
#else
    #define CHECK(X) X
#endif
