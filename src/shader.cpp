/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "common.h"
#include "utils.h"
#include "shader.h"

Shader::Shader(const string& vs, const string& fs)
{
    mProgram = glCreateProgram();

    // Compile vertex shader
    GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
    cout << "Shader: Compiling VS '" << vs << "'" << endl;
    string vsSource = utils::readFile(vs);
    const char* vsSourceData = vsSource.c_str();
    glShaderSource(vsID, 1, &vsSourceData, NULL);
    compileShader(vsID);
    glAttachShader(mProgram, vsID);

    // Compile fragment shader
    GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);
    cout << "Shader: Compiling FS '" << fs << "'" << endl;
    string fsSource = utils::readFile(fs);
    const char* fsSourceData = fsSource.c_str();
    glShaderSource(fsID, 1, &fsSourceData, NULL);
    compileShader(fsID);
    glAttachShader(mProgram, fsID);

    // Link program
    cout << "Shader: Linking shader program" << endl;
    glLinkProgram(mProgram);

    // Check the result of the link process
    GLint result = GL_FALSE;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        int infoLogLength;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* errorMessage = new char[infoLogLength];
        glGetProgramInfoLog(mProgram, infoLogLength, NULL, errorMessage);
        cout << "Error:" << errorMessage;
        delete[] errorMessage;
        return;
    }

    // Delete shaders
    glDeleteShader(vsID);
    glDeleteShader(fsID);
}

Shader::~Shader()
{
    if (mProgram != 0)
        glDeleteProgram(mProgram);
}


void Shader::bind()
{
    glUseProgram(mProgram);
}

void Shader::compileShader(GLuint id)
{
    GLint result;
    glCompileShader(id);

    // Check compilation result
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int infoLogLength;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

        char* errorMessage = new char[infoLogLength];
        glGetShaderInfoLog(id, infoLogLength, NULL, errorMessage);
        cout << "Error: " << errorMessage;
        delete[] errorMessage;
        
        // TODO: Error
    }
}
