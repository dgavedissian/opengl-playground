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

private:
    GLuint mProgram;
    
    void compileShader(GLuint id);

};

#endif /* SHADER_H */
