//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#include "matrix.h"
#include "vao.h"
#include "vertex_data.h"
//------------------------------------------------------------------------------
const Matrix::spawn Matrix::spawners[] = {
    &Matrix::spawn_a, &Matrix::spawn_b, &Matrix::spawn_c, &Matrix::spawn_d,
};
//------------------------------------------------------------------------------
Matrix::Matrix(std::size_t ns, std::size_t ng, TextureAtlas::Texture* texture)
: letter(texture)
, firsts(ns)
, counts(ns)
, nstrips(ns)
, nglyphs(ng)
, strips(ns)
, grid_random(324467)
, animation_period(100000000 + grandom(50000000U))
{
    const std::size_t strip_pack   = nglyphs * 4;
    const std::size_t num_vertices = nstrips * strip_pack;

    vector twister_pos(64.0f, -70.0f, -40.0f);

    vbo.bind();
    vbo.alloc(num_vertices, GL_STREAM_DRAW);
    {
        VBO<D4UB_V3F_C4F>::Map map(GL_WRITE_ONLY);

        VertexData::D4UB* glyphs    = map.address_of<0>(num_vertices);
        VertexData::V3F*  vertexies = map.address_of<1>(num_vertices);
        VertexData::C4F*  colors    = map.address_of<2>(num_vertices);

        for(unsigned int i = 0; i < nstrips; i++) {
            strips[i] = std::make_unique<Strip>(
                strip_pack, &glyphs[i * strip_pack], &vertexies[i * strip_pack],
                &colors[i * strip_pack], float(i), 2.5f - grid_random(5.0f),
                0.0f, twister_pos, -60.0f, 60.0f, grid_random(10.0f) + 2.0f,
                grid_random(20.0f) + 30.0f, grid_random(2.0f) + 10.0f,
                grid_random(20.0f) + 20.0f);
        }
    }

    vao.bind();
    vao.bind(vbo, GLuint(num_vertices), 0);
    vao.unbind();
    vbo.unbind();

    model.identity();
    model.translate(vector(-float(nstrips / 2), float(nglyphs / 2), -70.0f));
}

void Matrix::build_program()
{
    Shader        vshader(Shader::Vertex);
    const GLchar* vertex_shader = R"vs(
    #version 130
    uniform mat4 transform;
    in  uvec4 data;
    in  vec3 position;
    in  vec4 color;
    out vec4 ex_color;
    out vec2 tcoord;
    void main(void)
    {
        gl_Position = transform * vec4(position, 1.0);
        ex_color = color;
        //optimization of:
        //texcoord.t = ((gl_VertexID % 4) < 2) ? 1.0 : 0.0;
        tcoord.t = 0.15 + 0.7*((~gl_VertexID & 0x3) >> 1);
        tcoord.s = float( float(data.s) +  0.15 + 0.7*(float(gl_VertexID & 0x1)) )/64.0;
    })vs";

    vshader.set_source(vertex_shader);
    vshader.compile();
    vshader.log();

    Shader        fshader(Shader::Fragment);
    const GLchar* fragment_shader = R"fs(
    #version 130
    uniform sampler2D glyphs;
    in  vec4 ex_color;
    in  vec2 tcoord;
    out vec4 fragment;
    void main(void)
    {
        vec4 t = texture(glyphs, tcoord);
        if(t.r ==0) discard;
        fragment = ex_color*t.r;
    })fs";

    fshader.set_source(fragment_shader);
    fshader.compile();
    fshader.log();

    program.attach(vshader);
    program.attach(fshader);
    program.bind(0, "data");
    program.bind(1, "position");
    program.bind(2, "color");

    program.link();
    program.validate();
    program.log();
}

void Matrix::pre_draw(const Transform& transform)
{
    // No video
    // glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

    letter->bind();

    program.use();
    program.set_sampler("glyphs", 0);
    vao.bind();

    transform.bind_transform(program, model);
}

void Matrix::draw(const Transform& transform)
{
    pre_draw(transform);

    for(unsigned int i = 0; i < nstrips; i++) {
        strips[i]->draw(&firsts[i], &counts[i]);
        firsts[i] += GLint(i * nglyphs << 2);
    }

    glMultiDrawArrays(GL_TRIANGLE_STRIP, firsts.data(), counts.data(),
                      GLsizei(nstrips));
    /* for(unsigned int i = 0; i < nstrips; i++)
         glDrawArrays(GL_TRIANGLE_STRIP, firsts[i], counts[i]);
         */
    post_draw();
}

void Matrix::post_draw()
{
    vao.unbind();
    GPU_Program::use_default();
    glDisable(GL_BLEND);
    // glDepthMask(GL_TRUE);
}

void Matrix::tick(unsigned long usec)
{
    if(animation_period.ready())
    {
        // select random spawner
        unsigned int index = grandom(
            (unsigned int)(sizeof(Matrix::spawners) / sizeof(Matrix::spawn)));
        (this->*Matrix::spawners[index])();
    }

    const std::size_t strip_pack = nglyphs * 4;

    const std::size_t num_vertices = nstrips * strip_pack;

    vbo.bind();
    {
        VBO<D4UB_V3F_C4F>::Map map(GL_WRITE_ONLY);

        VertexData::D4UB* glyphs    = map.address_of<0>(num_vertices);
        VertexData::V3F*  vertexies = map.address_of<1>(num_vertices);
        VertexData::C4F*  colors    = map.address_of<2>(num_vertices);

        for(unsigned int i = 0; i < nstrips; i++) {
            strips[i]->tick(&glyphs[i * strip_pack], &vertexies[i * strip_pack],
                            &colors[i * strip_pack], usec);
        }
    }
    vbo.unbind();
}

void Matrix::spawn_a()
{
    for(unsigned int i = 0; i < nstrips; i++) {
        if(!strips[i]->arunning)
        {
            strips[i]->arunning = true;
#ifdef UNIX
            unsigned int delay = abs(i - nstrips / 2) * 120000;
#else
            unsigned int delay = abs(int(i - nstrips / 2)) * 120000;
#endif
            strips[i]->adelay = delay + grandom(30000U);
        }
    }
}

void Matrix::spawn_b()
{
    for(unsigned int i = 0; i < nstrips; i++) {
        if(!strips[i]->arunning)
        {
            strips[i]->arunning = true;
            unsigned int delay  = i * 80000;
            strips[i]->adelay   = delay + grandom(30000U);
        }
    }
}

void Matrix::spawn_c()
{
    for(unsigned int i = 0; i < nstrips; i++) {
        if(!strips[i]->arunning)
        {
            strips[i]->arunning = true;
            std::size_t delay   = (nstrips - i) * 80000;
            strips[i]->adelay   = delay + grandom(30000U);
        }
    }
}

void Matrix::spawn_d()
{
    for(unsigned int i = 0; i < nstrips; i++) {
        if(!strips[i]->arunning)
        {
            strips[i]->arunning = true;
            strips[i]->adelay   = grandom(2500000U);
        }
    }
}

Matrix::Strip::Strip(unsigned int n, VertexData::D4UB* glyphs,
                     VertexData::V3F* vertexies, VertexData::C4F* colors,
                     GLfloat x, GLfloat y, GLfloat z, const vector& ac,
                     float h1, float h2, float r, float p, float q,
                     float rotates)
: size(1.0f)
, animation(2048, size, vector(x, y, z), ac, h1, h2, r, p, q, rotates)
, wave_waiter(100000 + grandom(40000U))
, aframe_waiter(10000)
, n_glyphs(n / 4)
, end_glyph(0)
, wavehead(0)
, adelay(0)
, aframe(0)
, arunning(false)
{
    GLfloat yi = y;
    for(unsigned int i = 0; i < n; i += 4) {
        GLfloat a =
            cosf(float((int(wavehead - (i / 4)) % 80)) * (3.1415926f / 180.0f));

        GLubyte g = rand() % 64;

        glyphs[i].i    = g;
        colors[i].r    = 0.0f;
        colors[i].g    = 1.0f;
        colors[i].b    = 0.0f;
        colors[i].a    = a;
        vertexies[i].x = x;
        vertexies[i].y = yi;
        vertexies[i].z = z;

        glyphs[i + 1].i    = g;
        colors[i + 1].r    = 0.0f;
        colors[i + 1].g    = 1.0f;
        colors[i + 1].b    = 0.0f;
        colors[i + 1].a    = a;
        vertexies[i + 1].x = x + size;
        vertexies[i + 1].y = yi;
        vertexies[i + 1].z = z;

        glyphs[i + 2].i    = g;
        colors[i + 2].r    = 0.0f;
        colors[i + 2].g    = 1.0f;
        colors[i + 2].b    = 0.0f;
        colors[i + 2].a    = a;
        vertexies[i + 2].x = x;
        vertexies[i + 2].y = yi - size;
        vertexies[i + 2].z = z;

        glyphs[i + 3].i    = g;
        colors[i + 3].r    = 0.0f;
        colors[i + 3].g    = 1.0f;
        colors[i + 3].b    = 0.0f;
        colors[i + 3].a    = a;
        vertexies[i + 3].x = x + size;
        vertexies[i + 3].y = yi - size;
        vertexies[i + 3].z = z;

        yi -= size;
    }
}

void Matrix::Strip::draw(GLint* first, GLsizei* count)
{
    *first = 0;
    *count = end_glyph * 4;
}

void Matrix::Strip::tick(VertexData::D4UB* glyphs, VertexData::V3F* vertexies,
                         VertexData::C4F* colors, unsigned long usec)
{
    wave_tick(glyphs, vertexies, colors, usec);

    if(arunning)
    {
        if(adelay > 0)
        {
            adelay -= usec;
        }
        else
        {
            if(unsigned int iframe = aframe_waiter.test(usec))
            {
                aframe += iframe;
                if(aframe >= animation.nframes)// animation ends
                {
                    aframe   = 0;
                    arunning = false;// stop animation
                }

                //  end_glyph = n_glyphs;

                unsigned int w = animation.nframes - aframe;
                unsigned int k = n_glyphs;

                if(k > w)
                    end_glyph = 0;
                else
                    animation.vertexcpy(vertexies, k * 4, aframe);
            }
        }
    }
}

void Matrix::Strip::wave_tick(VertexData::D4UB* glyphs,
                              VertexData::V3F* /*vertexies*/,
                              VertexData::C4F* colors, unsigned long usec)
{
    if(unsigned int inc = wave_waiter.test(usec))
    {
        if(end_glyph < n_glyphs)
        {
            end_glyph += inc;
            if(end_glyph > n_glyphs)
                end_glyph = n_glyphs;
        }

        const unsigned int per = 64;                    // period of waves
        wavehead               = (wavehead + inc) % per;// clump to [0:per]

        assert(end_glyph <= n_glyphs);

        for(unsigned int i = 0; i < end_glyph; i++) {
            unsigned int index = i << 2;// index = i*4;
            unsigned int iper  = (wavehead - i) % per;
            if(iper != 0)
            {
                float   s = (float(iper) / float(per)) * 50.0f;
                GLfloat a = sinf(s * (3.1415926f / 180.0f));

                colors[index + 0].a = a;
                colors[index + 1].a = a;
                colors[index + 2].a = a;
                colors[index + 3].a = a;
            }
            else
            {
                // change glyph image
                GLubyte g = rand() % 64;

                glyphs[index + 0].i = g;
                glyphs[index + 1].i = g;//(g+1)%64;
                glyphs[index + 2].i = g;
                glyphs[index + 3].i = g;//(g+1)%64;

                colors[index + 0].a = 0.0f;
                colors[index + 1].a = 0.0f;
                colors[index + 2].a = 0.0f;
                colors[index + 3].a = 0.0f;
            }
        }
    }
}

MatrixVideo::MatrixVideo(unsigned int ns, unsigned int ng,
                         TextureAtlas::Texture* texture,
                         const VideoBuffer* buffer, bool vertical_flip,
                         bool horizontal_flip)
: Matrix(ns, ng, texture)
, video(buffer)
, vflip(vertical_flip)
, hflip(horizontal_flip)
{
}

void MatrixVideo::build_program()
{
    Shader        vshader(Shader::Vertex);
    const GLchar* vertex_shader = R"vs(
    #version 130
    uniform mat4 transform;
    uniform sampler2D video;
    in  uvec4 data;
    in  vec3 position;
    in  vec4 color;
    out vec4 ex_color;
    out vec2 tcoord;
    out vec2 vcoord;
    void main(void)
    {
        //gl_Position = transform * vec4(position, 1.0);
        ex_color = color;
        vcoord = vec2(1,0) + position.xy/(vec2(128.0, 96.0));
        //vcoord.x *= 0.6;
        vec4 v = texture(video, vcoord);
        float gray = dot(v.rgb, vec3(0.2125, 0.7154, 0.0721));
        gl_Position = transform * vec4(position.x, position.y, position.z - gray * 3, 1.0);
        // optimization of:
        //    texcoord.t = ((gl_VertexID % 4) < 2) ? 1.0 : 0.0;
        tcoord.t = 0.15 + 0.7*((~gl_VertexID & 0x3) >> 1);
        tcoord.s = float( float(data.s) +  0.15 + 0.7*(float(gl_VertexID & 0x1)) )/64.0;
    })vs";

    vshader.set_source(vertex_shader);
    vshader.compile();
    vshader.log();

    Shader        fshader(Shader::Fragment);
    const GLchar* fragment_shader = R"fs(
    #version 130
    uniform sampler2D glyphs;
    uniform sampler2D video;
    in vec4 ex_color;
    in  vec2 tcoord;
    in  vec2 vcoord;
    out vec4 fragment;
    void main(void)
    {
        vec4 t = texture(glyphs, tcoord);
        if(t.r == 0) discard;

        //vec2 factor = viewport.zw/textureSize(video, 0);
        //vec2 r = textureSize(video, 0) * max(factor.x, factor.y);

        //vec2 video_st = (gl_FragCoord.xy - viewport.zw / 2 + r* 1.5) /r;
        //vec4 v = texture(video, video_st);

        vec4 v = texture(video, vcoord);
        float gray = dot(v.rgb, vec3(0.2125, 0.7154, 0.0721));
        fragment = vec4(ex_color.rgb * min(t.r, gray), ex_color.a);
        //fragment = vec4( mix(ex_color.rgb, v.rgb, t.r), ex_color.a);
        //fragment = vec4(v.rgb, ex_color.a);
    "})fs";

    fshader.set_source(fragment_shader);
    fshader.compile();
    fshader.log();

    program.attach(vshader);
    program.attach(fshader);
    program.bind(0, "data");
    program.bind(1, "position");
    program.bind(2, "color");

    program.link();
    program.validate();
    program.log();
}

void MatrixVideo::pre_draw(const Transform& transform)
{
    //  Video & fixed pipeline
    Matrix::pre_draw(transform);

    glActiveTexture(GL_TEXTURE1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    vflip ? GL_MIRRORED_REPEAT : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    hflip ? GL_MIRRORED_REPEAT : GL_REPEAT);

    program.set_sampler("video", 1);

    video->frame()->bind();
}

void MatrixVideo::post_draw() { Matrix::post_draw(); }
//------------------------------------------------------------------------------
