//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   blas.h
// copyright:   (C) 2008, 2009, 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef BLAS_H
#define BLAS_H
//-----------------------------------------------------------------------------
#include <cmath>
#include <cstring>
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
struct matrix
{
public:
    union
    {
        struct { float array[16]; };
    };

    matrix(){}

    matrix(const matrix& m)
    {
        memcpy(array, m.array, sizeof(matrix));
    }
    matrix& operator=(const matrix& m)
    {
        memcpy(array, m.array, sizeof(matrix));
    }
    matrix(const float a[16])
    {
        memcpy(array, a, sizeof(matrix));
    }

    void identity(void)
    {
        memset(array, 0, sizeof(matrix));
        array[0] = array[5] = array[10] = array[15] = 1.0f;
    }

    void mul(const matrix& a, const matrix& b)
    {
        for(int i=0;i<4;++i)
            for(int j=0;j<4;++j)
            {
                for(int k=0;k<4;++k)
                    array[i*4+j] += a.array[i*4+k] * b.array[k*4+j];
            }
    }

    float det()const
    {
        float d12 = (array[2] * array[7]  - array[3] * array[6]);
        float d13 = (array[2] * array[11] - array[3] * array[10]);
        float d23 = (array[6] * array[11] - array[7] * array[10]);
        float d24 = (array[6] * array[15] - array[7] * array[14]);
        float d34 = (array[10]* array[15] - array[11]* array[14]);
        float d41 = (array[14]* array[3]  - array[15]* array[2]);

        return array[0] * (array[5] * d34- array[9] * d24 + array[13] * d23)
             - array[4] * (array[1] * d34 + array[9] * d41 + array[13] * d13)
             + array[8] * (array[1] * d24 + array[5] * d41 + array[13] * d12)
             - array[12]* (array[1] * d23 - array[5] * d13 + array[9] * d12);
    }

    matrix inverse()const
    {
        float d = det();
        matrix ret;
        if(!d) return ret;
        d = 1.0f / d;

        float d12 = (array[2]  * array[7]  - array[3]  * array[6]);
        float d13 = (array[2]  * array[11] - array[3]  * array[10]);
        float d23 = (array[6]  * array[11] - array[7]  * array[10]);
        float d24 = (array[6]  * array[15] - array[7]  * array[14]);
        float d34 = (array[10] * array[15] - array[11] * array[14]);
        float d41 = (array[14] * array[3]  - array[15] * array[2]);

        ret.array[0] =  (array[5] * d34 - array[9] * d24 + array[13] * d23);
        ret.array[1] = -(array[1] * d34 + array[9] * d41 + array[13] * d13);
        ret.array[2] =  (array[1] * d24 + array[5] * d41 + array[13] * d12);
        ret.array[3] = -(array[1] * d23 - array[5] * d13 + array[9]  * d12);
        ret.array[4] = -(array[4] * d34 - array[8] * d24 + array[12] * d23);
        ret.array[5] =  (array[0] * d34 + array[8] * d41 + array[12] * d13);
        ret.array[6] = -(array[0] * d24 + array[4] * d41 + array[12] * d12);
        ret.array[7] =  (array[0] * d23 - array[4] * d13 + array[8]  * d12);

        d12 = array[0] * array[5] - array[1] * array[4];
        d13 = array[0] * array[9] - array[1] * array[8];
        d23 = array[4] * array[9] - array[5] * array[8];
        d24 = array[4] * array[13]- array[5] * array[12];
        d34 = array[8] * array[13]- array[9] * array[12];
        d41 = array[12]* array[1] - array[13]* array[0];

        ret.array[8] =  (array[7] * d34 - array[11]* d24 + array[15] * d23);
        ret.array[9] = -(array[3] * d34 + array[11]* d41 + array[15] * d13);
        ret.array[10]=  (array[3] * d24 + array[7] * d41 + array[15] * d12);
        ret.array[11]= -(array[3] * d23 - array[7] * d13 + array[11] * d12);
        ret.array[12]= -(array[6] * d34 - array[10]* d24 + array[14] * d23);
        ret.array[13]=  (array[2] * d34 + array[10]* d41 + array[14] * d13);
        ret.array[14]= -(array[2] * d24 + array[6] * d41 + array[14] * d12);
        ret.array[15]=  (array[2] * d23 - array[6] * d13 + array[10] * d12);

        for(int i=0; i<16; ++i) ret.array[i] *= d;

        return ret;
    }

    void projection(float fov, float aspect, float znear, float zfar)
    {
        const float pi = 3.14159265f;

        float f = 1.0f / tanf(fov * pi / 360);
        float A = (zfar + znear) / (znear - zfar);
        float B = (2 * zfar * znear) / (znear - zfar);


        array[ 0] = f / aspect; array[ 1] =  0.0f; array[ 2] =  0.0f; array[ 3] =  0.0f;
        array[ 4] = 0.0f;       array[ 5] =  f;    array[ 6] =  0.0f; array[ 7] =  0.0f;
        array[ 8] = 0.0f;       array[ 9] =  0.0f; array[10] =  A;    array[11] =  B;
        array[12] = 0.0f;       array[13] =  0.0f; array[14] = -1;    array[15] =  0.0f;
    }

    static matrix rotate(const vector& v) // create rotation
    {
        vector rad = v * 0.01745329f;
        matrix r;
        r.identity();

        float sx = sin(rad.x), cx = cos(rad.x);
        float sy = sin(rad.y), cy = cos(rad.y);
        float sz = sin(rad.z), cz = cos(rad.z);

        r.array[0] = cy*cz; 
        r.array[1] = sx*sy*cz-cx*sz;
        r.array[2] = cx*sy*cz+sx*sz;

        r.array[4] = cy*sz; 
        r.array[5] = sx*sy*sz+cx*cz;
        r.array[6] = cx*sy*sz-sx*cz;

        r.array[8] = -sy;
        r.array[9] = sx*cy;
        r.array[10]= cx*cy;

        return r;
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
