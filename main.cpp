#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <ctime>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include "SDL.h"
#define ENABLE_IOSTREAM
#include "transform.h"
#include "canvas.h"

typedef enum { TRIANGLE_STRIP, LINE_STRIP, LINE_LOOP, POINTS } prim_t;

class Renderer {
    Canvas &m_canvas;
    std::vector<vec4f> m_vertices;
    std::vector<vec2f> m_texcoords;
    const Pixman *m_texture;
    Matrix4f m_viewport;
    Matrix4f m_model;
    Matrix4f m_trans;

    void drawPoints()
    {
        for (unsigned i = 0; i < m_vertices.size(); i++) {
            vec4f dot = m_trans * m_vertices[i];
            dot /= dot.w();
            m_canvas.plot(dot.x(), dot.y());
        }
    }

    void drawLines(bool loop = true)
    {
        vec2i p0, p1, p2;

        for (unsigned i = 0; i < m_vertices.size(); i++) {
            vec4f dot = m_trans * m_vertices[i];
            dot /= dot.w();

            if (i == 0) {
                p0 = p1 = dot;
                continue;
            }

            p2 = dot;
            m_canvas.line(p1.x(), p1.y(), p2.x(), p2.y());
            p1 = p2;

            if (loop && (i+1 == m_vertices.size()))
                m_canvas.line(p1.x(), p1.y(), p0.x(), p0.y());
        }
    }

    void drawTriangleStrip()
    {
        bool texmap = m_texture &&
            m_vertices.size() == m_texcoords.size();

        Vertex vt[3];
        memset(&vt, 0, sizeof(vt));

        for (unsigned i = 0, n = 0;
             i < m_vertices.size();
             i++, n++)
        {
            vec4f pos = m_trans * m_vertices[i];
            pos /= pos.w();

            vt[n].x = pos.x();
            vt[n].y = pos.y();
            if (texmap) {
                vt[n].u = (m_texture->width()-1)*m_texcoords[i].x();
                vt[n].v = (m_texture->height()-1)*m_texcoords[i].y();
            }

            if (n < 2)
                continue;

            m_canvas.triangle(vt);
            vt[0] = vt[1];
            vt[1] = vt[2];
            n = 1;
        }
    }

public:
    Renderer(Canvas &canvas)
        : m_canvas(canvas)
        , m_texture(NULL)
    {
        float sx = m_canvas.width()/2;
        float sy = m_canvas.height()/2;

        m_viewport = scale(sx, -sy, 1.0f) * translate(1.0f, -1.0f, 0.0f);
    }

    void transform(const Matrix4f &m)
    {
        m_model = m * m_model;
    }

    void vertexPointer(const float pp[][3], int count)
    {
        m_vertices.clear();
        for (int i = 0; i < count; i++)
            m_vertices.push_back(vec4f(pp[i][0], pp[i][1], pp[i][2], 1));
    }

    void texcoordPointer(const float tt[][2], int count)
    {
        m_texcoords.clear();
        for (int i = 0; i < count; i++)
            m_texcoords.push_back(vec2f(tt[i][0], tt[i][1]));
    }

    void texture(const Pixman *texture)
    {
        m_texture = texture;
        m_canvas.texture(texture);
    }

    void render(prim_t mode)
    {
        Matrix4f proj;
        proj[2][3] = 1;
        proj[3][3] = 0;
        m_trans = m_viewport * proj * translate(0.f, 0.f, 1.f) * m_model;

        switch (mode) {
        case TRIANGLE_STRIP:
            drawTriangleStrip();
            break;
        case LINE_LOOP:
            drawLines();
            break;
        case LINE_STRIP:
            drawLines(false);
            break;
        case POINTS:
            drawPoints();
            break;
        }
    }
};

Pixman test_texture(const PixelFormat &format)
{
    int size = 10;
    Pixman tex(size, size, format);

    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++) {
            uint32_t color;
            if ((y+x) & 1)
                color = tex.mapRGB(0x00, 0x00, 0x00);
            else
                color = tex.mapRGB(0xFF, 0xFF, 0xFF);
            tex.set(x, y, color);
        }

    return tex;
}

Pixman sdlPixman(SDL_Surface *sdlSurface)
{
    SDL_PixelFormat *sdlFormat = sdlSurface->format;
    PixelFormat pf;
    pf.bpp = sdlFormat->BytesPerPixel;
    pf.mR = sdlFormat->Rmask;
    pf.mG = sdlFormat->Gmask;
    pf.mB = sdlFormat->Bmask;
    pf.mA = sdlFormat->Amask;
    pf.sR = sdlFormat->Rshift;
    pf.sG = sdlFormat->Gshift;
    pf.sB = sdlFormat->Bshift;
    pf.sA = sdlFormat->Ashift;

    return Pixman(sdlSurface->w, sdlSurface->h,
                  pf, (uint8_t*)sdlSurface->pixels);
}


int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Surface *screen = SDL_SetVideoMode(640, 480, 24, SDL_SWSURFACE|SDL_DOUBLEBUF);
    assert(screen != NULL);
    Pixman pscreen = sdlPixman(screen);
    Pixman texture = test_texture(pscreen.format());

    Canvas canvas(pscreen);
    Renderer r(canvas);
    // Example data

    float pp[][3] = {
        {-0.5, -0.5, 0},
        {-0.5, 0.5, 0},
        {0.5, -0.5, 0},
        {0.5, 0.5, 0},

        {0.5, 0.5, 1.0},
        {-0.5, 0.5, 1.0}
    };

    float tt[][2] = {
        {0.0, 0.0},
        {0.0, 1.0},
        {1.0, 0.0},
        {1.0, 1.0},

        {0.0, 0.0},
        {0.0, 1.0},
    };

    r.vertexPointer(pp, 6);
    r.texcoordPointer(tt, 6);
    r.texture(&texture);

    //r.transform(rotate(1.f, 0.f, 1.f, 0.f));
    r.render(POINTS);
    r.render(TRIANGLE_STRIP);
    //r.render(LINE_LOOP);

    //scaling_copy(pscreen, screen->w/2, screen->h/2, texture);
    SDL_Flip(screen);

    bool run = true;
    while (run) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
        case SDL_QUIT:
            run = false;
            break;
        default:
            continue;
        }
    }

    SDL_Quit();
	return 0;
}
