#ifndef CANVAS_H
#define CANVAS_H

#include "SDL.h"
#include "vec.h"

class Canvas {
    SDL_Surface *m_surface;
    uint32_t m_color;
    uint32_t m_mask;
public:
    Canvas(SDL_Surface *surf)
        : m_surface(surf)
    {
        color(0xFF, 0x00, 0x00);
        colorMask(true, true, true, true);
    }

    void color(uint8_t r, uint8_t g, uint8_t b);
    void colorMask(bool r, bool g, bool b, bool a);

    void plot(int x, int y);
    void line(int x1, int y1, int x2, int y2);
    void triangle(const vec2i v[3]);
    void swapBuffers();

    int width()
    {
        return m_surface->w;
    }
    int height()
    {
        return m_surface->h;
    }

    void lock()
    {
        SDL_LockSurface(m_surface);
    }

    void unlock()
    {
        SDL_UnlockSurface(m_surface);
    }
};

#endif
