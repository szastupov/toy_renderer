#ifndef PIXMAN_H
#define PIXMAN_H

#include "SDL.h"

class Pixman {
    uint8_t *colors;
    uint16_t w, h;
    unsigned pitch;
    uint16_t bpp;
    uint8_t mR, mG, mB, mA;
    uint8_t sR, sG, sB, sA;
    bool allocated;

    void initSDL(SDL_PixelFormat *sdlFormat)
    {
        bpp = sdlFormat->BytesPerPixel;
        pitch = bpp*w;
        mR = sdlFormat->Rmask;
        mG = sdlFormat->Gmask;
        mB = sdlFormat->Bmask;
        mA = sdlFormat->Amask;
        sR = sdlFormat->Rshift;
        sG = sdlFormat->Gshift;
        sB = sdlFormat->Bshift;
        sA = sdlFormat->Ashift;
    }

    uint32_t* pixel(uint16_t x, uint16_t y) const
    {
        return (uint32_t*)(colors + y*pitch + x*bpp);
    }

public:
    Pixman(uint16_t sw, uint16_t sh, SDL_PixelFormat *sdlFormat)
    {
        w = sw;
        h = sh;
        initSDL(sdlFormat);
        colors = (uint8_t*)malloc(h*pitch);
        allocated = true;
    }

    Pixman(SDL_Surface *sdlSurface)
    {
        w = sdlSurface->w;
        h = sdlSurface->h;
        initSDL(sdlSurface->format);
        colors = (uint8_t*)sdlSurface->pixels;
        allocated = false;
    }

    ~Pixman()
    {
        if (allocated)
            free(colors);
    }

    uint16_t width()
    {
        return w;
    }

    uint16_t height()
    {
        return h;
    }

    uint32_t mapRGB(uint8_t r, uint8_t g, uint8_t b)
    {
        return r << sR | g << sG | b << sB | mA;
    }

    uint8_t mapRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        return r << sR | g << sG | b << sB | (a << sA & mA);
    }

    uint32_t get(uint32_t x, uint32_t y) const
    {
        return *pixel(x, y);
    }

    void set(uint32_t x, uint32_t y, uint32_t color)
    {
        *pixel(x, y) = color;
    }
};

#endif
