//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gpu_program.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef GPU_PROGRAM
#define GPU_PROGRAM
//-----------------------------------------------------------------------------
#include "gl_renderer.h"
//-----------------------------------------------------------------------------
class Shader
{
    friend class GPU_Program;
public:
    enum  shader_type{ Vertex=GL_VERTEX_SHADER, Fragment=GL_FRAGMENT_SHADER };

    inline Shader(shader_type type)
    {
        handle = glCreateShader(type);
    }

    inline ~Shader()
    {
        glDeleteShader(handle);
    }

    inline void set_source(const GLchar* source)
    {
        glShaderSource(handle, 1, &source, NULL);
    }

    bool compile();
    void log();
private:
    GLuint handle;
};

class GPU_Program
{
public:
    inline GPU_Program()
    {
        handle = glCreateProgram();
    }

    inline ~GPU_Program()
    {
        glDeleteProgram(handle);
    }

    inline void attach(const Shader& shader)
    {
        glAttachShader(handle, shader.handle);
    }

    inline void detach(const Shader& shader)
    {
        glDetachShader(handle, shader.handle);
    }

    bool link();
    void log();
    bool validate();
    bool use();

    void set_sampler(const char* name, int value);
    void set_uniform(const char* name, float a);
    void set_uniform(const char* name, float a[4]);

    static inline void use_default(){ glUseProgram(0); }

private:
    GLuint handle;
};
//-----------------------------------------------------------------------------
#endif//GPU_PROGRAM
//-----------------------------------------------------------------------------
