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

	GLuint vsID = compileShader(VERTEX_SHADER, vs);
	glAttachShader(mProgram, vsID);
	GLuint fsID = compileShader(FRAGMENT_SHADER, fs);
    glAttachShader(mProgram, fsID);

	linkProgram();

    // Delete shaders now that they've been linked
    glDeleteShader(vsID);
    glDeleteShader(fsID);
}

Shader::~Shader()
{
    if (mProgram)
        glDeleteProgram(mProgram);
}

void Shader::bind()
{
    glUseProgram(mProgram);
}

GLuint Shader::compileShader(ShaderType type, const string& sourceFile)
{
	// Output to the log
	INFO << "Compiling Shader ";
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
	string source = utils::readEntireFile(sourceFile);
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
		ERROR << "Shader Compile Error: " << errorMessage;
		delete[] errorMessage;

		// TODO: Error
	}

	return id;
}

void Shader::linkProgram()
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
		ERROR << "Shader Link Error:" << errorMessage;
		delete[] errorMessage;
	}
}

GLint Shader::getUniformLocation(const string& name)
{
    GLint location = glGetUniformLocation(mProgram, name.c_str());
    if (location == -1)
        WARNING << "Unable to find uniform '" << name << "'" << endl;
    return location;
}
