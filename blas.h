//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   blas.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef BLAS_H
#define BLAS_H
//-----------------------------------------------------------------------------
#include <cmath>
//-----------------------------------------------------------------------------
struct vector
{
    union
    {
        struct{ float xyz[3]; };
        struct{ float x,y,z;  };
    };

    vector(){}
    vector(float a, float b, float c):x(a), y(b), z(c)
    {
    }
    vector(const vector& v):x(v.x), y(v.y), z(v.z)
    {
    }

    vector operator*(float a)const
    {
        return vector(x*a, y*a, z*a);
    }

    vector operator/(float a)const
    {
        return vector(x/a, y/a, z/a);
    }

    vector operator+(const vector& v)const
    {
        return vector(x+v.x, y+v.y, z+v.z);
    }

    vector operator-(const vector& v)const
    {
        return vector(x-v.x, y-v.y, z-v.z);
    }

    vector& operator=(float v[3])
    {
        x = v[0];
        y = v[1];
        z = v[2];
        return *this;
    }

    vector& operator=(const vector& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

};

//-----------------------------------------------------------------------------
class HSpline   // Charles Hermite`s spline
{
public:
    HSpline(const vector* p, unsigned int n):points(p), num(n)
    {
    }

    ~HSpline()
    {
    }

    vector operator()(float t)const
    {
        float ptime = t*(num-3);
        int index = int(ptime);

        float u =  ptime - int(ptime); // u - local time [0.0 <= u <= 1.0]
        float u2 = u*u;
        float u3 = u2*u;

        vector v =  points[index+1]                 * (2.0f*u3 - 3.0f*u2+1.0f)  +
                   (points[index+2]-points[index])  * (u3 - 2.0f*u2 + u)        +
                    points[index+2]                 * (-2.0f*u3 + 3.0f*u2)      +
                   (points[index+3]-points[index+1])* (u3-u2);
        return v;
    }

private:
    const vector* points;
    unsigned int num;
};
//-----------------------------------------------------------------------------
#endif//BLAS_H
//-----------------------------------------------------------------------------
