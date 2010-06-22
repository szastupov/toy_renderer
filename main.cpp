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
    Transform<float> m_trans;
public:
    Renderer(SDL_Surface *screen)
        : m_screen(screen)
    {
        float sx = m_screen->w/2;
        float sy = m_screen->h/2;

        m_trans.rotate(2, 1, 1, 1);

        // Camera
        //m_trans.translate(0, 0, 1);

        // Apply viewport
        m_trans.translate(1, 1, 0).scale(sx, sy, 1);

        dots.push_back(vec4f(-0.5, -0.5, 0, 1));
        dots.push_back(vec4f(0.5, -0.5, 0, 1));
        dots.push_back(vec4f(0.5, 0.5, 0, 1));
        dots.push_back(vec4f(-0.5, 0.5, 0, 1));
    }

    void render()
    {
        SDL_PixelFormat *format = m_screen->format;
        int bpp = format->BytesPerPixel;
        int pitch = m_screen->pitch;

#define RGB(r, g, b) (r << format->Rshift | g << format->Gshift | b << format->Bshift)

        SDL_LockSurface(m_screen);

        uint8_t *pixels = (uint8_t*)m_screen->pixels;
        for (unsigned i = 0; i < dots.size(); i++)
        {
            vec4f dot = m_trans * dots[i];
            uint32_t *p = (uint32_t*)(pixels+(int)dot.y()*pitch+(int)dot.x()*bpp);
            *p = RGB(0xFF, 0x00, 0x00);
        }

        SDL_UnlockSurface(m_screen);
        SDL_Flip(m_screen);
    }
};

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Surface *screen = SDL_SetVideoMode(640, 480, 24, SDL_HWSURFACE|SDL_DOUBLEBUF);
    Renderer r(screen);
    r.render();

    bool run = true;
    while (run)
    {
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type)
        {
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
