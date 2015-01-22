/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "common.h"
#include "utils.h"
#include "shader.h"

Shader::Builder::Builder()
{
}

Shader::Builder::~Builder()
{
}

Shader::Builder& Shader::Builder::vs(const string& file)
{
    mVertexShader = file;
    return *this;
}

Shader::Builder& Shader::Builder::fs(const string& file)
{
    mFragmentShader = file;
    return *this;
}

shared_ptr<Shader> Shader::Builder::link()
{
    return make_shared<Shader>(*this);
}

Shader::Shader(Shader::Builder& builder)
{
    mProgram = glCreateProgram();

    // Compile vertex shader
    GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
    cout << "Shader: Compiling VS '" << builder.mVertexShader << "'" << endl;
    string vsSource = utils::readFile(builder.mVertexShader);
    const char* vsSourceData = vsSource.c_str();
    glShaderSource(vsID, 1, &vsSourceData, NULL);
    compileShader(vsID);
    glAttachShader(mProgram, vsID);

    // Compile fragment shader
    GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);
    cout << "Shader: Compiling FS '" << builder.mFragmentShader << "'" << endl;
    string fsSource = utils::readFile(builder.mFragmentShader);
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

GLint Shader::getUniform(const string& name)
{
    return glGetUniformLocation(mProgram, name.c_str());
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
