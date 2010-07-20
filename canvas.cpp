#include <cstdio>
#include <algorithm>
#include <cassert>

#include "canvas.h"

void Canvas::setColor(uint8_t r, uint8_t g, uint8_t b)
{
    m_color = m_surface.mapRGB(r, g, b);
}

void Canvas::plot(int x, int y, int z, uint32_t color)
{
    if (x < 0 || x > m_surface.width() ||
        y < 0 || y > m_surface.height())
        return;

    if (z <= m_zBuffer[y*m_surface.height()+x]) {
        m_surface.set(x, y, color);
        m_zBuffer[y*m_surface.height()+x] = z;
    }
}

void Canvas::point(int x, int y, int z)
{
    plot(x, y, z, m_color);
}

static bool cmpY(const Vertex &a, const Vertex &b)
{
    return a.y() < b.y();
}

static bool cmpX(const Vertex &a, const Vertex &b)
{
    return a.x() < b.x();
}

void Canvas::line(const Vertex &a, const Vertex &b)
{
    Vertex v[2] = { a, b };

    if (a.y() == b.y()) {
        std::sort(v, v+2, cmpX);
        return straightLineX(v[0].x(), v[1].x(), a.y());
    }

    if (a.x() == b.x()) {
        std::sort(v, v+2, cmpY);
        return straightLineY(v[0].y(), v[1].y(), a.x());
    }

    std::sort(v, v+2, cmpX);
    float dx = v[1].x()-v[0].x();
    float dy = (v[1].y()-v[0].y())/dx;
    float y = v[0].y();

    // FIXME: z
    for (int x = v[0].x(); x <= v[1].x(); x++) {
        plot(x, y, 0, m_color);
        y += dy;
    }
}

// FIXME The next two functions should use z-buffer too
void Canvas::straightLineX(int x1, int x2, int y)
{
    for (int x = x1; x <= x2; x++)
        plot(x, y, 0, m_color);
}

void Canvas::straightLineY(int y1, int y2, int x)
{
    for (int y = y1; y <= y2; y++)
        plot(x, y, 0, m_color);
}

// UP_DOWN - for flat bottom
// DOWN_UP - for float top
enum { UP_DOWN, DOWN_UP };

void Canvas::scanlineTriangle(const Vertex vt[3], int dir)
{
    // Indeces specify requred order
    static const int idx1[3] = { 0, 1, 2};
    static const int idx2[3] = { 2, 0, 1};
    const int *idx;
    int l0, r0;

    if (dir == UP_DOWN) {
        idx = idx1;
        l0 = r0 = 0;
    } else {
        idx = idx2;
        l0 = 1;
        r0 = 2;
    }

    float dy = vt[idx[2]].y() - vt[idx[0]].y();

    // Prepare vectors for interpolation
    vec4f v[3];
    for (int i = 0; i < 3; i++) {
        int j = idx[i];
        v[i][0] = vt[j][0]; // x
        v[i][1] = vt[j][3]; // u
        v[i][2] = vt[j][4]; // v
        v[i][3] = vt[j][2]; // z
    }
    if (v[1].x() > v[2].x())
        std::swap(v[1][0], v[2][0]);

    vec4f dvl = (v[1]-v[0])/dy; // Change in left line
    vec4f dvr = (v[2]-v[0])/dy; // Change in right line
    vec4f vl = v[l0];           // Left interpolant
    vec4f vr = v[r0];           // Right interpolant

    // Interpolate me baby!
    for (int y = vt[0].y(); y <= vt[2].y(); y++) {
        vec4f ddv = vr-vl;
        // Change in uvz
        vec3f duvz = vec3(ddv[1], ddv[2], ddv[3]);
        duvz /= ddv.x();
        vec3f uvz = vec3(vl[1], vl[2], vl[3]);
        for (int x = vl.x(); x <= vr.x(); x++) {
            int z = uvz[2]*100;
            // FIXME: remove branch here
            plot(x, y, z, m_texture ? m_texture->get(uvz[0], uvz[1]) : m_color);
            uvz += duvz;
        }
        vl += dvl;
        vr += dvr;
    }
}

static Vertex lerpY(const Vertex &a, const Vertex &b, int y)
{
    Vertex d = b-a;
    return a+(d/d.y())*y;
}

void Canvas::triangle(const Vertex vs[3])
{
    Vertex vt[3];
    std::copy(vs, vs+3, vt);
    std::sort(vt, vt+3, cmpY);

    if (vt[0].y() == vt[2].y())
        return;                 // Empty triangle

    if (vt[0].y() == vt[1].y())
        scanlineTriangle(vt, DOWN_UP);
    else if (vt[1].y() == vt[2].y())
        scanlineTriangle(vt, UP_DOWN);
    else {
        // Make two "flat" triangles
        Vertex vh[3];

        int hy = vt[1].y();
        Vertex h = lerpY(vt[0], vt[2], hy);
        h[1] = hy;

        vh[0] = vt[0];
        vh[1] = vt[1];
        vh[2] = h;
        scanlineTriangle(vh, UP_DOWN);

        vh[0] = h;
        vh[1] = vt[1];
        vh[2] = vt[2];
        scanlineTriangle(vh, DOWN_UP);
    }
}
