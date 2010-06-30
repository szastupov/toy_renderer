#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <ctime>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include "transform.h"
#include "SDL.h"

#define SWAP(x, y) { x ^= y; y ^= x; x ^= y; }

typedef std::vector<vec4f> VertexArray;

class Renderer
{
    SDL_Surface *m_screen;
    const VertexArray *m_vertices;
    Matrix4f m_viewport;
    Matrix4f m_model;
    uint32_t m_color;

    void drawPoints(const Matrix4f &trans)
    {
        const VertexArray &dots = *m_vertices;
        for (unsigned i = 0; i < dots.size(); i++) {
            vec4f dot = trans * dots[i];
            dot = dot/dot.w();
            plot(dot.x(), dot.y());
        }
    }

    void bLine(int x1, int y1, int x2, int y2)
    {
        bool steep = abs(y2 - y1) > abs(x2 - x1);
        if (steep) {
            SWAP(x1, y1);
            SWAP(x2, y2);
        }
        if (x1 > x2) {
            SWAP(x1, x2);
            SWAP(y1, y2);
        }

        int dx = x2-x1,
            dy = abs(y2-y1),
            error = dx >> 1,
            ystep = y1 < y2 ? 1 : -1;

        for (int x = x1, y = y1; x <= x2; x++) {
            if (steep)
                plot(y, x);
            else
                plot(x, y);
            error -= dy;
            if (error < 0) {
                y += ystep;
                error += dx;
            }
        }
    }

    void drawLines(const Matrix4f &trans, bool loop = true)
    {
        const VertexArray &dots = *m_vertices;
        int x0, y0, x1, y1, x2, y2;

        for (unsigned i = 0; i < dots.size(); i++) {
            vec4f dot = trans * dots[i];
            dot = dot/dot.w();

            if (i == 0) {
                x0 = x1 = dot[0];
                y0 = y1 = dot[1];
                continue;
            }

            x2 = dot[0];
            y2 = dot[1];
            bLine(x1, y1, x2, y2);
            x1 = x2; y1 = y2;

            if (loop && (i+1 == dots.size()))
                bLine(x1, y1, x0, y0);
        }
    }

    void minmax(const vec2i v[3], int &cmax, int &cmin, int c)
    {
        cmax = v[0][c];
        cmin = v[0][c];
        for (int i = 1; i < 3; i++) {
            cmax = std::max(v[i][c], cmax);
            cmin = std::min(v[i][c], cmin);
        }
    }

    bool online(const vec2i &a, const vec2i &b, int y)
    {
        return (a.y() == y && b.y() == y);
    }

    bool cross(const vec2i &a, const vec2i &b, int y)
    {
        return ((a.y() > y && b.y() < y) ||
                (a.y() < y && b.y() > y));
    }

    int get_x(const vec2i &a, const vec2i &b, int y)
    {
        vec2i d = b-a;
        return (float)(y-a.y()) / ((float)d.y()/(float)d.x()) + a.x();
    }

    void drawTriangle(const vec2i v[3])
    {
        int miny, maxy;
        minmax(v, maxy, miny, 1);

        int e[][2] = {
            {0, 1},
            {1, 2},
            {2, 0}
        };

        for (int y = miny; y <= maxy; y++) {
            bool crossed = false;
            for (int i = 0; i < 3; i++) {
                if (cross(v[e[i][0]], v[e[i][1]], y)) {
                    if (crossed) {
                        int x2 = get_x(v[e[i][0]], v[e[i][1]], y),
                            x1 = get_x(v[e[i-1][0]], v[e[i-1][1]], y);
                        for (int x = x1; x <= x2; x++)
                            plot(x, y);
                    } else
                        crossed = true;
                } else if (online(v[e[i][0]], v[e[i][1]], y)) {
                    int x1 = v[e[i][0]].x(),
                        x2 = v[e[i][1]].x();
                    for (int x = x2; x <= x1; x++)
                        plot(x, y);
                }
            }
        }
    }

    void drawTriangleStrip(const Matrix4f &trans)
    {
        const VertexArray &dots = *m_vertices;
        vec2i v[3];

        for (unsigned i = 0, step = 0;
             i < dots.size();
             i++, step++)
        {
            vec4f dot = trans * dots[i];
            dot = dot/dot.w();

            v[step][0] = dot[0];
            v[step][1] = dot[1];
            if (step < 2)
                continue;

            drawTriangle(v);
            v[0] = v[1];
            v[1] = v[2];
            step = 1;
        }
    }

    void plot(int x, int y)
    {
        if (x < 0 || x > m_screen->w ||
            y < 0 || y > m_screen->h)
            return;

        SDL_PixelFormat *format = m_screen->format;
        int bpp = format->BytesPerPixel;
        int pitch = m_screen->pitch;
        uint8_t *pixels = (uint8_t*)m_screen->pixels + y*pitch + x*bpp;

        *(uint32_t*)pixels = m_color;
    }

public:
    Renderer(SDL_Surface *screen)
        : m_screen(screen)
    {
        float sx = m_screen->w/2;
        float sy = m_screen->h/2;
        color(0xFF, 0xFF, 0xFF);

        m_viewport = scale(sx, -sy, 1.0f) * translate(1.0f, -1.0f, 0.0f);
        m_model = rotate(1.f, 1.f, 0.f, 0.f);
    }

    void vertexPointer(const VertexArray *vp)
    {
        m_vertices = vp;
    }

    void color(uint8_t r, uint8_t g, uint8_t b)
    {
        SDL_PixelFormat *format = m_screen->format;
        m_color = r << format->Rshift | g << format->Gshift | b << format->Bshift;
    }

    void render()
    {
        Matrix4f proj;
        proj[2][3] = 1;
        proj[3][3] = 0;
        Matrix4f trans = m_viewport * proj * translate(0.f, 0.f, 1.f) * m_model;

        SDL_LockSurface(m_screen);

        drawTriangleStrip(trans);
        //drawLines(trans);
        //drawPoints(trans);

        SDL_UnlockSurface(m_screen);
        SDL_Flip(m_screen);
    }

};

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Surface *screen = SDL_SetVideoMode(640, 480, 24, SDL_HWSURFACE|SDL_DOUBLEBUF);

    Renderer r(screen);
    // Example data
    float pp[][3] = {
        {-0.5, -0.5, 0},
        {-0.5, 0.5, 0},
        {0.5, -0.5, 0},
        {0.5, 0.5, 0}
    };
    std::vector<vec4f> dots;
    for (int i = 0; i < 4; i++)
        dots.push_back(vec4f(pp[i][0], pp[i][1], pp[i][2], 1));
    r.color(0xFF, 0x00, 0x00);
    r.vertexPointer(&dots);
    r.render();

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
