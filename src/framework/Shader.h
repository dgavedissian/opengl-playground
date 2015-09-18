/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#pragma once

enum ShaderType
{
	VERTEX_SHADER = GL_VERTEX_SHADER,
	GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
	FRAGMENT_SHADER = GL_FRAGMENT_SHADER
};

class Shader
{
public:
    Shader(const string& vs, const string& fs);
    ~Shader();

    // Bind this shader
    void Bind();

    // Set uniform parameters
    template <class T> void SetUniform(const string& name, const T& value);

private:
    GLuint mProgram;

	GLuint CompileShader(ShaderType type, const string& sourceFile);
	void LinkProgram();

    GLint GetUniformLocation(const string& name);

};

template <> inline void Shader::SetUniform(const string& name, const int& value)
{
    glUniform1i(GetUniformLocation(name), value);
}

template <> inline void Shader::SetUniform(const string& name, const float& value)
{
    glUniform1f(GetUniformLocation(name), value);
}

template <> inline void Shader::SetUniform(const string& name, const glm::vec2& value)
{
    glUniform2f(GetUniformLocation(name), value.x, value.y);
}

template <> inline void Shader::SetUniform(const string& name, const glm::vec3& value)
{
    glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}

template <> inline void Shader::SetUniform(const string& name, const glm::vec4& value)
{
    glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
}

template <> inline void Shader::SetUniform(const string& name, const glm::mat2& value)
{
    glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

template <> inline void Shader::SetUniform(const string& name, const glm::mat3& value)
{
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

template <> inline void Shader::SetUniform(const string& name, const glm::mat4& value)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
