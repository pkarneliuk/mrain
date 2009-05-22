//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gpu_program.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cstdio>
#include <cstring>

#include "gpu_program.h"
//-----------------------------------------------------------------------------
bool Shader::compile()
{
    glCompileShader(handle);
    GLint success = GL_FALSE;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    return GL_TRUE == success;
}

void Shader::log()
{
    GLint size = 0;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &size);
    GLchar* info = new GLchar[size];
    memset(info, '\0', sizeof(GLchar) * size);
    glGetShaderInfoLog(handle, size, NULL, info );
    fprintf(stderr, "Shader log:\n%s\n", info);
    delete[] info;
}

bool GPU_Program::link()
{
    glLinkProgram(handle);
    GLint success = GL_FALSE;
    glGetProgramiv(handle, GL_LINK_STATUS, &success);
    return success == GL_TRUE;
}

void GPU_Program::log()
{
    GLint size = 0;
    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &size);
    GLchar* info = new GLchar[size];
    memset(info, '\0', sizeof(GLchar) * size);
    glGetProgramInfoLog(handle, size, NULL, info );
    fprintf(stderr, "Program log:\n%s\n", info);
    delete[] info;
}

bool GPU_Program::validate()
{
    glValidateProgram(handle);
    GLint success = GL_FALSE;
    glGetProgramiv(handle, GL_VALIDATE_STATUS, &success);
    return GL_TRUE == success;
}

bool GPU_Program::use()
{
    glUseProgram(handle);
    return GL_NO_ERROR == glGetError();
}

void GPU_Program::set_sampler(const char* name, int value)
{
    GLint location = glGetUniformLocation(handle, name);
    glUniform1i( location, value );
}

void GPU_Program::set_uniform(const char* name, float a)
{
    GLint location = glGetUniformLocation(handle, name);
    glUniform1f( location, a );
}

void GPU_Program::set_uniform(const char* name, float a[4])
{
    GLint location = glGetUniformLocation(handle, name);
    glUniform4fv( location,1, a );
}
//-----------------------------------------------------------------------------
