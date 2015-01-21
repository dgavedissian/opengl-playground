/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "common.h"
#include "utils.h"
#include "shader.h"

ShaderBuilder::ShaderBuilder()
{
}

ShaderBuilder::~ShaderBuilder()
{
}

ShaderBuilder& ShaderBuilder::vs(const string& file)
{
    mVertexShader = file;
    return *this;
}

ShaderBuilder& ShaderBuilder::fs(const string& file)
{
    mFragmentShader = file;
    return *this;
}

shared_ptr<Shader> ShaderBuilder::link()
{
    shared_ptr<Shader> out = make_shared<Shader>();
    out->_build(mVertexShader, mFragmentShader);
    return out;
}

Shader::Shader() : mProgram(0)
{
}

Shader::~Shader()
{
    if (mProgram != 0)
        glDeleteProgram(mProgram);
}

void Shader::_build(const string& vsFile, const string& fsFile)
{
    GLint result = GL_FALSE;
    GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile vertex shader
    cout << "Shader: Compiling VS '" << vsFile << "'" << endl;
    string vsSource = utils::readFile(vsFile);
    const char* vsSourceData = vsSource.c_str();
    glShaderSource(vsID, 1, &vsSourceData, NULL);
    glCompileShader(vsID);

    // Check vertex shader
    glGetShaderiv(vsID, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int infoLogLength;
        glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* errorMessage = new char[infoLogLength];
        glGetShaderInfoLog(vsID, infoLogLength, NULL, errorMessage);
        cout << "Error: " << errorMessage;
        delete[] errorMessage;
        return;
    }

    // Compile fragment shader
    cout << "Shader: Compiling FS '" << fsFile << "'" << endl;
    string fsSource = utils::readFile(fsFile);
    const char* fsSourceData = fsSource.c_str();
    glShaderSource(fsID, 1, &fsSourceData, NULL);
    glCompileShader(fsID);

    // Check fragment shader
    glGetShaderiv(fsID, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int infoLogLength;
        glGetShaderiv(fsID, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* errorMessage = new char[infoLogLength];
        glGetShaderInfoLog(fsID, infoLogLength, NULL, errorMessage);
        cout << "Error: " << errorMessage;
        delete[] errorMessage;
        return;
    }

    // Link program
    cout << "Shader: Linking shader program" << endl;
    mProgram = glCreateProgram();
    glAttachShader(mProgram, vsID);
    glAttachShader(mProgram, fsID);
    glLinkProgram(mProgram);

    // Check the program
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

void Shader::bind()
{
    glUseProgram(mProgram);
}
