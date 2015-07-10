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

	GLuint vsID = CompileShader(VERTEX_SHADER, vs);
	glAttachShader(mProgram, vsID);
	GLuint fsID = CompileShader(FRAGMENT_SHADER, fs);
    glAttachShader(mProgram, fsID);

	LinkProgram();

    // Delete shaders now that they've been linked
    glDeleteShader(vsID);
    glDeleteShader(fsID);
}

Shader::~Shader()
{
    if (mProgram)
        glDeleteProgram(mProgram);
}

void Shader::Bind()
{
    glUseProgram(mProgram);
}

GLuint Shader::CompileShader(ShaderType type, const string& sourceFile)
{
	// Output to the log
	cout << "[Shader] Compiling ";
	switch (type)
	{
	case VERTEX_SHADER:
		cout << "VS";
		break;

	case GEOMETRY_SHADER:
		cout << "GS";
		break;

	case FRAGMENT_SHADER:
		cout << "FS";
		break;

	default:
		cout << "<unknown type>";
	}
	cout << " '" << sourceFile << "'" << endl;

	// Create shader
	GLuint id = glCreateShader((GLuint)type);

	// Read source code
	string source = utils::ReadEntireFile(sourceFile);
	const char* sourceFileData = source.c_str();
	glShaderSource(id, 1, &sourceFileData, NULL);

	// Compile
	glCompileShader(id);

	// Check compilation result
	GLint result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int infoLogLength;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

		char* errorMessage = new char[infoLogLength];
		glGetShaderInfoLog(id, infoLogLength, NULL, errorMessage);
		cout << "[Shader] [ERROR] Compile Error: " << errorMessage;
		delete[] errorMessage;

		// TODO: Error
	}

	return id;
}

void Shader::LinkProgram()
{
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
		cout << "[Shader] [ERROR] Link Error:" << errorMessage;
		delete[] errorMessage;
	}
}

GLint Shader::GetUniformLocation(const string& name)
{
    GLint location = glGetUniformLocation(mProgram, name.c_str());
    if (location == -1)
        cerr << "[Shader] [WARNING] Unable to find uniform '" << name << "'" << endl;
    return location;
}
