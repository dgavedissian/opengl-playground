/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "Common.h"
#include "Utils.h"
#include "Shader.h"

Shader::Shader(const string& vs, const string& fs)
{
    mProgram = glCreateProgram();

    // Compile vertex shader
    GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
    cout << "[Shader] Compiling VS '" << vs << "'" << endl;
    string vsSource = utils::ReadEntireFile(vs);
    const char* vsSourceData = vsSource.c_str();
    glShaderSource(vsID, 1, &vsSourceData, NULL);
    CompileShader(vsID);
    glAttachShader(mProgram, vsID);

    // Compile fragment shader
    GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);
    cout << "[Shader] Compiling FS '" << fs << "'" << endl;
    string fsSource = utils::ReadEntireFile(fs);
    const char* fsSourceData = fsSource.c_str();
    glShaderSource(fsID, 1, &fsSourceData, NULL);
    CompileShader(fsID);
    glAttachShader(mProgram, fsID);

    // Link program
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
        cout << "[Shader] Link Error:" << errorMessage;
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

void Shader::Bind()
{
    glUseProgram(mProgram);
}

void Shader::CompileShader(GLuint id)
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
        cout << "[Shader] Compile Error: " << errorMessage;
        delete[] errorMessage;
        
        // TODO: Error
    }
}

GLint Shader::GetUniformLocation(const string& name)
{
    GLint location = glGetUniformLocation(mProgram, name.c_str());
    if (location == -1)
        cerr << "[Shader] Unable to find uniform " << name << endl;
    return location;
}
