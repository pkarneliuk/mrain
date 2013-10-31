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

    Transform(float f, float zn, float zf) : fov(f), znear(zn), zfar(zf)
    {
        proj.identity();
        view.identity();
    }
    ~Transform(){}

    Transform(const Transform&);      // undefined
    void operator=(const Transform&); // undefined

    void set_viewport(int x, int y, unsigned int width, unsigned int height)
    {
        viewport[0] = float(x);
        viewport[1] = float(y);
        viewport[2] = float(width);
        viewport[3] = float(height);

        set_perspective(fov, znear, zfar);
    }

    void set_perspective(float f, float zn, float zf)
    {
        fov     = f;
        znear   = zn;
        zfar    = zf;
        const float aspect = viewport[2]/viewport[3]; // width/height

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

    const matrix& get_proj() const { return proj; }
    const matrix& get_view() const { return view; }
    const matrix& get_projview() const { return projview; }

    void bind_transform(GPU_Program& program, const matrix& model) const;
    void bind_viewport(GPU_Program& program) const;

private:

    matrix proj;
    matrix view;
    matrix projview; // proj * view
    float viewport[4]; // x, y, width, height

    float fov;
    float znear;
    float zfar;
};
//-----------------------------------------------------------------------------
#endif//TRANSFORM_H
//-----------------------------------------------------------------------------