#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <ctime>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include "transform.h"
#include "canvas.h"

typedef std::vector<vec4f> VertexArray;

class Renderer {
    SDL_Surface *m_screen;
    const VertexArray *m_vertices;
    Matrix4f m_viewport;
    Matrix4f m_model;
    Matrix4f m_trans;
    Canvas m_canvas;

    void drawPoints(const Matrix4f &trans)
    {
        const VertexArray &dots = *m_vertices;
        for (unsigned i = 0; i < dots.size(); i++) {
            vec4f dot = trans * dots[i];
            dot = dot/dot.w();
            m_canvas.plot(dot.x(), dot.y());
        }
    }

    void drawLines(const Matrix4f &trans, bool loop = true)
    {
        const VertexArray &dots = *m_vertices;
        vec2i p0, p1, p2;

        for (unsigned i = 0; i < dots.size(); i++) {
            vec4f dot = trans * dots[i];
            dot = dot/dot.w();

            if (i == 0) {
                p0 = p1 = dot;
                continue;
            }

            p2 = dot;
            m_canvas.line(p1.x(), p1.y(), p2.x(), p2.y());
            p1 = p2;

            if (loop && (i+1 == dots.size()))
                m_canvas.line(p1.x(), p1.y(), p0.x(), p0.y());
        }
    }

    void drawTriangleStrip()
    {
        const VertexArray &dots = *m_vertices;
        vec2i v[3];

        for (unsigned i = 0, step = 0;
             i < dots.size();
             i++, step++)
        {
            vec4f dot = m_trans * dots[i];
            dot = dot/dot.w();

            v[step] = dot;
            if (step < 2)
                continue;

            m_canvas.triangle(v);
            v[0] = v[1];
            v[1] = v[2];
            step = 1;
        }
    }

public:
    Renderer(SDL_Surface *screen)
        : m_screen(screen)
        , m_canvas(screen)
    {
        float sx = m_screen->w/2;
        float sy = m_screen->h/2;

        m_viewport = scale(sx, -sy, 1.0f) * translate(1.0f, -1.0f, 0.0f);
        m_model = rotate(1.f, 1.f, 0.f, 0.f);
    }

    void vertexPointer(const VertexArray *vp)
    {
        m_vertices = vp;
    }

    void render()
    {
        Matrix4f proj;
        proj[2][3] = 1;
        proj[3][3] = 0;
        m_trans = m_viewport * proj * translate(0.f, 0.f, 1.f) * m_model;

        SDL_LockSurface(m_screen);

        drawTriangleStrip();
        //drawLines();
        //drawPoints();

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
