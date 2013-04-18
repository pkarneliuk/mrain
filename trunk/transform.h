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
#include "gpu_program.h"
//-----------------------------------------------------------------------------
class Transform
{
public:

    Transform()
    {
        proj.identity();
        view.identity();
    }
    ~Transform(){}

    Transform(const Transform&);      // undefined
    void operator=(const Transform&); // undefined

    void set_perspective(float fov, float aspect, float znear, float zfar)
    {
        proj.perspective(fov, aspect, znear, zfar);
        projview.mul(proj, view);
    }

    void set_translate(const vector& position)
    {
        view.translate(position);
        projview.mul(proj, view);
    }

    void set_rotate(const vector& orientation)
    {
        view.rotate(orientation);
        projview.mul(proj, view);
    }

    const matrix& get_proj()const { return proj; }
    const matrix& get_view()const { return view; }
    const matrix& get_projview()const { return projview; }

    void bind_to(GPU_Program& program, const matrix& model)const;

private:

    matrix proj;
    matrix view;
    matrix projview; // proj * view
};
//-----------------------------------------------------------------------------
#endif//TRANSFORM_H
//-----------------------------------------------------------------------------