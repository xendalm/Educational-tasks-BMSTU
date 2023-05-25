#include <math.h>
#include <GL/glut.h>
#include <iostream>
#define PI 3.14159265359

double rotate_y = 0;
double rotate_x = 0;
double l = 1, phi = PI / 4;

void display()
{
    // prism points
    unsigned int n = 9;
    float radius = 50.0;
    float segment = 2 * PI / n;
    struct point
    {
        float x, y, z;
    } a[12], b[12];
    for (int i = 0; i < n + 2; i++)
    {
        float x = radius * sin(i * segment);
        float y = radius * cos(i * segment);
        a[i].x = x;
        a[i].y = y;
        a[i].z = -100;
        b[i].x = x;
        b[i].y = y;
        b[i].z = 100;
    };
    // display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLdouble matrix[16] = {1, 0, 0, 0,
                           0, 1, 0, 0,
                           -l * cos(phi), -l * sin(phi), 0, 0,
                           0, 0, 0, 1};
    glMultMatrixd(matrix);
    glLineWidth(3);
    glColor3d(0, 0, 1);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < n + 2; i++)
        glVertex3f(a[i].x, a[i].y, a[i].z);
    glEnd();
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < n + 2; i++)
        glVertex3f(b[i].x, b[i].y, b[i].z);
    glEnd();
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < n + 2; i++)
    {
        glVertex3f(a[i].x, a[i].y, a[i].z);
        glVertex3f(b[i].x, b[i].y, b[i].z);
        glVertex3f(a[i].x, a[i].y, a[i].z);
    }
    glEnd();
    glutSwapBuffers();
}

void specialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_RIGHT)
    {
        l -= 0.05;
    }
    else if (key == GLUT_KEY_LEFT)
    {
        l += 0.05;
    }
    else if (key == GLUT_KEY_UP)
    {
        phi += PI / 60;
    }
    else if (key == GLUT_KEY_DOWN)
    {
        phi -= PI / 60;
    }
    glutPostRedisplay();
}

void Initialize()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-200, 200, 200, -200, -200, 200);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Test");
    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    Initialize();
    glutMainLoop();
    return 0;
}