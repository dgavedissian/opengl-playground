/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef SHADER_H
#define SHADER_H

class Shader
{
public:
    Shader(const string& vs, const string& fs);
    ~Shader();
    
    // Bind this shader
    void bind();

    // Set uniform parameters
    template <class T> void setUniform(const string& name, const T& value);

private:
    GLuint mProgram;
    
    void compileShader(GLuint id);
    GLint getUniformLocation(const string& name);

};

template <> inline void Shader::setUniform(const string& name, const int& value)
{
    glUniform1i(getUniformLocation(name), value);
}

template <> inline void Shader::setUniform(const string& name, const float& value)
{
    glUniform1f(getUniformLocation(name), value);
}

template <> inline void Shader::setUniform(const string& name, const glm::vec2& value)
{
    glUniform2f(getUniformLocation(name), value.x, value.y);
}

template <> inline void Shader::setUniform(const string& name, const glm::vec3& value)
{
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

template <> inline void Shader::setUniform(const string& name, const glm::vec4& value)
{
    glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

template <> inline void Shader::setUniform(const string& name, const glm::mat2& value)
{
    glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

template <> inline void Shader::setUniform(const string& name, const glm::mat3& value)
{
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

template <> inline void Shader::setUniform(const string& name, const glm::mat4& value)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

#endif /* SHADER_H */
