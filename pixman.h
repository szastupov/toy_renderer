#ifndef PIXMAN_H
#define PIXMAN_H

#include <cstring>
#include <stdint.h>

struct PixelFormat {
    uint16_t bpp;
    uint8_t mR, mG, mB, mA;
    uint8_t sR, sG, sB, sA;
};

class Pixman {
    const PixelFormat pf;
    uint16_t w, h;
    unsigned pitch;
    size_t size;
    uint8_t *colors;
    bool allocated;

    uint32_t* pixel(uint16_t x, uint16_t y) const
    {
        return (uint32_t*)(colors + y*pitch + x*pf.bpp);
    }

public:
    Pixman(uint16_t sw, uint16_t sh,
           const PixelFormat &pf,
           uint8_t *scolors = NULL) :
        pf(pf),
        w(sw), h(sh),
        pitch(pf.bpp*w),
        size(h*pitch+1),
        colors(scolors ? scolors : new uint8_t[size]),
        allocated(!scolors)
    {}

    Pixman(const Pixman &src) :
        pf(src.pf),
        w(src.w),
        h(src.h),
        pitch(src.pitch),
        size(src.size),
        colors(new uint8_t[size]),
        allocated(true)
    {
        memcpy(colors, src.colors, h*pitch);
    }

    ~Pixman()
    {
        if (allocated)
            delete [] colors;
    }

    uint16_t width() const
    {
        return w;
    }
    uint16_t height() const
    {
        return h;
    }

    const PixelFormat& format() const
    {
        return pf;
    }

    uint32_t mapRGB(uint8_t r, uint8_t g, uint8_t b)
    {
        return r << pf.sR | g << pf.sG | b << pf.sB | pf.mA;
    }

    uint8_t mapRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        return r << pf.sR | g << pf.sG | b << pf.sB | (a << pf.sA & pf.mA);
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
