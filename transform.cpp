//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   transform.cpp
// copyright:   (C) 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "transform.h"
//-----------------------------------------------------------------------------
void Transform::bind_to(GPU_Program& program, const matrix& model)const
{
    matrix tmp;
    tmp.mul(projview, model);
    program.set_uniform_matrix("transform", tmp.array);
}
//-----------------------------------------------------------------------------