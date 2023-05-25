#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#define PI 3.14159265358979323846

double rotate_y = 0;
double rotate_x = 0;
double l = 1, phi = PI / 4, k = 1;
bool mode = false;
using namespace std;

void display()
{
    if (mode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(-0.5, 0.0, 0.0);
    glScalef(0.5, 0.5, 0.5);
    glPushMatrix();
    glScalef(0.5, 0.5, 0.5);
    GLdouble matrix[16] = {1, 0, 0, 0,
                           0, 1, 0, 0,
                           l * cos(phi), l * sin(phi), 1, 0,
                           0, 0, 0, 1};
    glMultMatrixd(matrix);
    glTranslatef(-0.5, -0.5, 0.5 * l);
    glBegin(GL_LINES);
    glColor3d(1, 0, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(2, 0, 0);
    glColor3d(0, 1, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 2, 0);
    glColor3d(0, 0, 1);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, -2);
    glEnd();
    glTranslatef(0.5, 0.5, -0.5 * l);
    // BACK
    glBegin(GL_POLYGON);
    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5 * l * k);
    glVertex3f(0.5, 0.5, 0.5 * l * k);
    glVertex3f(-0.5, 0.5, 0.5 * l * k);
    glVertex3f(-0.5, -0.5, 0.5 * l * k);
    glEnd();
    // TOP
    glBegin(GL_POLYGON);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.5, 0.5, 0.5 * l * k);
    glVertex3f(0.5, 0.5, -0.5 * l * k);
    glVertex3f(-0.5, 0.5, -0.5 * l * k);
    glVertex3f(-0.5, 0.5, 0.5 * l * k);
    glEnd();
    // RIGHT
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.5, -0.5, -0.5 * l * k);
    glVertex3f(0.5, 0.5, -0.5 * l * k);
    glVertex3f(0.5, 0.5, 0.5 * l * k);
    glVertex3f(0.5, -0.5, 0.5 * l * k);
    glEnd();
    // FRONT
    glBegin(GL_LINE_LOOP);
    glColor3d(0, 0, 1);
    glVertex3f(0.5, -0.5, -0.5 * l * k);
    glVertex3f(0.5, 0.5, -0.5 * l * k);
    glVertex3f(-0.5, 0.5, -0.5 * l * k);
    glVertex3f(-0.5, -0.5, -0.5 * l * k);
    glEnd();
    glPopMatrix();
    // CUBE
    glTranslatef(2, 0.0, 0.0);
    glRotatef(-20, 1.0, 0.0, 0.0);
    glRotatef(40, 0.0, 1.0, 0.0);
    glScalef(0.5, 0.5, 0.5);
    // FRONT
    glLineWidth(1);
    glBegin(GL_LINE_STRIP);
    glColor3d(0, 0, 1);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);
    glEnd();
    // BACK
    glBegin(GL_POLYGON);
    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(-0.5, -0.5, 0.5);
    glEnd();
    // RIGHT
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glEnd();
    // TOP
    glBegin(GL_POLYGON);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glEnd();
    glutSwapBuffers();
}

void specialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_RIGHT)
    {
        l -= 0.2;
    }
    else if (key == GLUT_KEY_LEFT)
    {
        l += 0.2;
    }
    else if (key == GLUT_KEY_UP)
    {
        phi += PI / 24;
    }
    else if (key == GLUT_KEY_DOWN)
    {
        phi -= PI / 24;
    }
    else if (key == GLUT_KEY_F1)
    {
        mode = !mode;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(700, 700);
    glutCreateWindow("Test");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    return 0;
}