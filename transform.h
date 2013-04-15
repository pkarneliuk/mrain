//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   transform.h
// copyright:   (C) 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef TRANSFORM_H
#define TRANSFORM_H
//-----------------------------------------------------------------------------
#include "blas.h"
#include "gl_context.h"
#include "stuff.h"
//-----------------------------------------------------------------------------
class Transform
{
public:

    Transform()
    {
        projection.identity();
        modelview.identity();
        transform.identity();
    }
    ~Transform(){}

    Transform(const Transform&);      // undefined
    void operator=(const Transform&); // undefined

    void set_perspective(float fov, float aspect, float znear, float zfar)
    {
        projection.perspective(fov, aspect, znear, zfar);
        transform.mul(projection, modelview);
    }

    void set_translate(const vector& position)
    {
        modelview.translate(position);
        transform.mul(projection, modelview);
    }

    void set_rotate(const vector& orientation)
    {
        modelview.rotate(orientation);
        transform.mul(projection, modelview);
    }

    const matrix& get_projection()const { return projection; }
    const matrix& get_modelview ()const { return modelview;  }
    const matrix& get_transform ()const { return transform;  }

private:

    matrix projection;
    matrix modelview;
    matrix transform;   // projection * modelview
};
//-----------------------------------------------------------------------------
#endif//TRANSFORM_H
//-----------------------------------------------------------------------------