/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#ifndef SHADER_H
#define SHADER_H

class Shader;

class ShaderBuilder
{
public:
    ShaderBuilder();
    ~ShaderBuilder();

    ShaderBuilder& vs(const string& file);
    ShaderBuilder& fs(const string& file);
    shared_ptr<Shader> link();

private:
    string mVertexShader;
    string mFragmentShader;

};

class Shader
{
public:
    Shader();
    ~Shader();

    // Used by ShaderBuilder
    void _build(const string& vs, const string& fs);

    // Bind this shader
    void bind();

private:
    GLuint mProgram;

};

#endif /* SHADER_H */
