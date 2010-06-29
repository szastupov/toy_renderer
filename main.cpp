#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <ctime>
#include <vector>
#include <sys/time.h>
#include "transform.h"
#include "SDL.h"

class Renderer
{
    SDL_Surface *m_screen;
    std::vector<vec4f> dots;
    Matrix4f m_viewport;
    Matrix4f m_model;
public:
    Renderer(SDL_Surface *screen)
        : m_screen(screen)
    {
        float sx = m_screen->w/2;
        float sy = m_screen->h/2;

        m_viewport = scale(sx, -sy, 1.0f) * translate(1.0f, -1.0f, 0.0f);
        m_model = rotate(1.f, 1.f, 0.f, 0.f);

        // Example data
        float pp[][3] = {
            {-0.5, -0.5, 0},
            {-0.5, 0.5, 0},
            {0.5, 0.5, 0},
            {0.5, -0.5, 0}
        };
        for (int i = 0; i < 4; i++)
            dots.push_back(vec4f(pp[i][0], pp[i][1], pp[i][2], 1));
    }

    void drawDots(const Matrix4f &trans)
    {
        for (unsigned i = 0; i < dots.size(); i++) {
            vec4f dot = trans * dots[i];
            dot = dot/dot.w();
            if (dot.x() < 0 || dot.x() > m_screen->w ||
                dot.y() < 0 || dot.y() > m_screen->h)
                continue;
            setPixel(dot.x(), dot.y(), 0xFF, 0x00, 0x00);
        }
    }

    void drawLines(const Matrix4f &trans)
    {
        float x1, y1, x2, y2;
        for (unsigned i = 0; i < dots.size(); i++) {
            vec4f dot = trans * dots[i];
            dot = dot/dot.w();
            if (dot.x() < 0 || dot.x() > m_screen->w ||
                dot.y() < 0 || dot.y() > m_screen->h)
                continue;

            if (i == 0) {
                x1 = dot[0];
                y1 = dot[1];
            } else {
                x2 = dot[0];
                y2 = dot[1];
                float dx = x2-x1;
                float dy = y2-y1;

                if (dx == 0) {
                    for (float iy = y1; iy <= y2; iy++)
                        setPixel(x1, iy, 0xFF, 0x00, 0x00);
                } else {
                    for (float x = x1; x <= x2; x++) {
                        float y = y1 + (dy) * (x - x1)/dx;
                        setPixel(x, y, 0xFF, 0x00, 0x00);
                    }
                }
                x1 = x2; y1 = y2;
            }
        }
    }

    void render()
    {
        Matrix4f proj;
        proj[2][3] = 1;
        proj[3][3] = 0;
        Matrix4f trans = m_viewport * proj * translate(0.f, 0.f, 1.f) * m_model;

        SDL_LockSurface(m_screen);

        drawLines(trans);

        SDL_UnlockSurface(m_screen);
        SDL_Flip(m_screen);
    }

    inline void setPixel(unsigned x, unsigned y, uint8_t r, uint8_t g, uint8_t b)
    {
        SDL_PixelFormat *format = m_screen->format;
        int bpp = format->BytesPerPixel;
        int pitch = m_screen->pitch;
        uint8_t *pixels = (uint8_t*)m_screen->pixels + y*pitch + x*bpp;

        *(uint32_t*)pixels = r << format->Rshift | g << format->Gshift | b << format->Bshift;
    }

};

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Surface *screen = SDL_SetVideoMode(640, 480, 24, SDL_HWSURFACE|SDL_DOUBLEBUF);
    Renderer r(screen);
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
