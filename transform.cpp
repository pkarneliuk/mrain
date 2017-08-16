//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "transform.h"
#include "gpu_program.h"
//------------------------------------------------------------------------------
void Transform::bind_transform(GPU_Program& program, const matrix& model) const
{
    matrix tmp;
    tmp.mul(projview, model);
    program.set_uniform_matrix("transform", tmp.array);
}

void Transform::bind_viewport(GPU_Program& program) const
{
    program.set_uniform("viewport", viewport);
}
//------------------------------------------------------------------------------