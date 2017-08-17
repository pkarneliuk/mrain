//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "gl_context.h"
//------------------------------------------------------------------------------
namespace VertexData
{
    // clang-format off
    struct D1UB
    {
        enum{ num=1, type=GL_INT+GL_UNSIGNED_BYTE };
        union
        {
            struct{ GLubyte data[num]; };
            struct{ GLubyte i;         };
        };
    };

    struct D4UB
    {
        enum{ num=4, type=GL_INT+GL_UNSIGNED_BYTE };
        union
        {
            struct{ GLubyte data[num]; };
            struct{ GLubyte i,j,m,n;   };
        };
    };

    struct T2F
    {
        enum{ num=2, type=GL_FLOAT   };
        union
        {
            struct{ GLfloat st[num]; };
            struct{ GLfloat s,t;     };
        };
    };

    struct T2US
    {
        enum{ num=2, type=GL_UNSIGNED_SHORT };
        union
        {
            struct{ GLushort st[num]; };
            struct{ GLushort s,t;     };
        };
    };

    struct V2F
    {
        enum{ num=2, type=GL_FLOAT    };
        union
        {
            struct{ GLfloat xy[num]; };
            struct{ GLfloat x,y;     };
        };
    };

    struct V3F
    {
        enum{ num=3, type=GL_FLOAT    };
        union
        {
            struct{ GLfloat xyz[num]; };
            struct{ GLfloat x,y,z;    };
        };
    };

    struct C3F
    {
        enum{ num=3, type=GL_FLOAT    };
        union
        {
            struct{ GLfloat rgb[num]; };
            struct{ GLfloat r,g,b;    };
        };
    };

    struct C4F
    {
        enum{ num=4, type=GL_FLOAT     };
        union
        {
            struct{ GLfloat rgba[num]; };
            struct{ GLfloat r,g,b,a;   };
        };
    };

    struct C4UB
    {
        enum{ num=4, type=GL_UNSIGNED_BYTE };
        union
        {
            struct{ GLubyte rgba[num]; };
            struct{ GLubyte r,g,b,a;   };
        };
    };

    // Examples of usage:
    // typedef Layout<V3F, V3F, T2F, C4UB> VervexNormalTexCoordColor;
    // int offset = VervexNormalTexCoordColor::index<2>::offset;
    template <typename M1=void,
              typename M2=void,
              typename M3=void,
              typename M4=void,
              typename M5=void,
              typename M6=void,
              typename M7=void,
              typename M8=void>
    class Layout
    {
    public:
        template<typename T, typename U>
        struct Node
        {
            using element = T;
            using next    = U;
        };

        typedef Node<M1,
                Node<M2,
                Node<M3,
                Node<M4,
                Node<M5,
                Node<M6,
                Node<M7,
                Node<M8,
                Node<void, void>  > > > >  > > > > list;

    private:
        // Index
        template<typename L, unsigned int Offset, std::size_t i>
        struct Index
        {
        private:
            using Next = Index<typename L::next, Offset+sizeof(typename L::element), i-1>;
        public:
            enum { offset = Next::offset };
            using type = typename Next::type;
        };

        template<typename L, unsigned int Offset>
        struct Index<L, Offset, 0>
        {
            enum { offset = Offset };
            using type = typename L::element;
        };

        // Size
        template<typename T, typename U>
        struct size
        {
            enum { value = sizeof(T) + size<typename U::element, typename U::next>::value };
        };

        template<typename U>
        struct size<void, U>
        {
            enum { value = 0 };
        };
        char dummy[size<typename list::element, typename list::next>::value]; //for valid sizeof(Layout)

        Layout() = delete;

    public:
        template<std::size_t i> struct index:public Index<list, 0, i>{};
    };
    // clang-format on
}
//------------------------------------------------------------------------------