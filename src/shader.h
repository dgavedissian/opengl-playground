/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef SHADER_H
#define SHADER_H

class Shader
{
public:
    class Builder
    {
    public:
        friend class Shader;

        Builder();
        ~Builder();

        Builder& vs(const string& file);
        Builder& fs(const string& file);
        shared_ptr<Shader> link();

    private:
        string mVertexShader;
        string mFragmentShader;
    };

    Shader(Builder& builder);
    ~Shader();
    
    // Bind this shader
    void bind();

    // Get uniform parameter location
    GLint getUniform(const string& name);

    // Set uniform parameters
    template <class T> void setUniform(GLint location, const T& value);

private:
    GLuint mProgram;
    
    void compileShader(GLuint id);

};

template <> inline void Shader::setUniform(GLint location, const float& value)
{
    glUniform1f(location, value);
}

template <> inline void Shader::setUniform(GLint location, const glm::vec2& value)
{
    glUniform2f(location, value.x, value.y); }

template <> inline void Shader::setUniform(GLint location, const glm::vec3& value)
{
    glUniform3f(location, value.x, value.y, value.z);
}

template <> inline void Shader::setUniform(GLint location, const glm::vec4& value)
{
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

template <> inline void Shader::setUniform(GLint location, const glm::mat2& value)
{
    glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

template <> inline void Shader::setUniform(GLint location, const glm::mat3& value)
{
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

template <> inline void Shader::setUniform(GLint location, const glm::mat4& value)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

#endif /* SHADER_H */
