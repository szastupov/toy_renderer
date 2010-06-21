#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <ctime>
#include <vector>
#include <GL/glut.h>
#include <sys/time.h>
#include "vec.h"
#include "matrix.h"

struct Particle {
    vec3f pos;
    vec3f vel;
};

time_t time_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000+tv.tv_usec/1000;
}

class ParticleSystem {
    std::vector<Particle> particles;
    time_t update_time;
    float mx, my;
    int w, h;

public:
    ParticleSystem()
        : particles(60)
        , update_time(time_ms())
    {
    }

    void boom()
    {
        int size = particles.size();
        int half = size/2;
        float speed = 0.4;

        for (int i = 0; i < half; i++)
        {
            float a = (2*M_PI/half)*i;
            vec3f disp(cos(a), sin(a), 0);
            particles[i].vel += disp*speed;
        }
        for (int i = 0; i < half; i++) {
            float a = (2*M_PI/half)*i;
            vec3f disp(0, sin(a), cos(a));
            particles[half+i].vel += disp*speed;
        }
    }

    void reshape(int nw, int nh)
    {
        w = nw;
        h = nh;
    }

    void move(int x, int y)
    {
        mx = ((float)x/w*2)-1;
        my = 1-((float)y/h*2);
    }

    void render()
    {
        int s = particles.size();
        GLfloat points[s][3];
        for (int i = 0; i < s; i++)
        {
            points[i][0] = particles[i].pos[0];
            points[i][1] = particles[i].pos[1];
            points[i][2] = particles[i].pos[2];
        }

        glPointSize(5);
        glVertexPointer(3, GL_FLOAT, 0, points);
        glDrawArrays(GL_POINTS, 0, s);
    }

    void update()
    {
        time_t cur_time = time_ms();
        float dt = (float)(cur_time-update_time)/1000;
        update_time = cur_time;

        int s = particles.size();
        for (int i = 0; i < s; i++)
        {
            float speed = 0.5;
            vec3f disp(mx, my, 0);
            disp -= particles[i].pos;
            if (disp[0] || disp[1])
                disp.normalize();
            particles[i].vel += disp*speed*dt;

            particles[i].pos += particles[i].vel*dt;
        }
    }
};

ParticleSystem ps;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
    //glRotatef(20, 0, 1, 1);

    ps.render();

	glutSwapBuffers();
}

void tick(int data)
{
    ps.update();
	glutPostRedisplay();

	glutTimerFunc(50, tick, 0);
}

void init(void)
{
	glEnable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
}

void motion(int x, int y)
{
    ps.move(x, y);
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    ps.reshape(w, h);
}

void click(int button, int state, int x, int y)
{
    ps.boom();
}

void vec_test()
{
#define ASSERT(e) printf("%-40s = %s\n", #e, (e) ? "ok" : "fail");
    vec2f a(1, 2);
    vec2f b(2, 4);
    vec2f c(1, 2);
    vec2f d(-1, -2);

    ASSERT(a != b);
    ASSERT(a == c);
    ASSERT(a+c == b);
    ASSERT(d == -a);
    ASSERT(a*2 == b);
    ASSERT(b/2 == a);
    ASSERT(a[0] == 1 && a[1] == 2);
    ASSERT(a[0] == a.x() && a[1] == a.y());
    ASSERT(b.length() != 0);
    ASSERT(b.normalized() < vec2f(1, 1));
    ASSERT(dot(a, b) == 10);

#undef ASSERT
}

int main(int argc, char **argv)
{
    matrix_test();
    return 0;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("simple");

    glutMouseFunc(click);
    glutReshapeFunc(reshape);
    glutPassiveMotionFunc(motion);
	glutDisplayFunc(display);
	glutTimerFunc(50, tick, 0);

	init();

	glutMainLoop();
	return 0;
}
