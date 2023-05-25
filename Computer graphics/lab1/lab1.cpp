#include <stdlib.h>
#include <GL/glut.h>

float red = 1.0f, blue = 1.0f, green = 1.0f, anglex = 0.0f, angley = 0.0f, ycoord = 0.0f, xcoord = 0.0f;

void changeSize(int w, int h)
{
    if (h == 0)
        h = 1;
    float ratio = w * 1.0 / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 10.0f,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);
    glRotatef(angley, 0.0f, 1.0f, 0.0f);
    glRotatef(anglex, 1.0f, 0.0f, 0.0f);
    glColor3f(red, green, blue);
    glBegin(GL_TRIANGLES);

    glVertex3f(-2.0f + xcoord, -2.0f + ycoord, 0.0f);
    glVertex3f(0.0f + xcoord, 2.0f + ycoord, 0.0);
    glVertex3f(2.0f + xcoord, -2.0f + ycoord, 0.0);

    glVertex3f(-1.7f + xcoord, -1.0f + ycoord, 0.0f);
    glVertex3f(0.0f + xcoord, 3.0f + ycoord, 0.0);
    glVertex3f(1.7f + xcoord, -1.0f + ycoord, 0.0);

    glVertex3f(-1.5f + xcoord, 0.0f + ycoord, 0.0f);
    glVertex3f(0.0f + xcoord, 3.0f + ycoord, 0.0);
    glVertex3f(1.5f + xcoord, 0.0f + ycoord, 0.0);

    glEnd();
    glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y)
{
    if (key == 27)
        exit(0);
}

void processSpecialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_F1:
        red = 1.0;
        green = 0.0;
        blue = 0.0;
        break;
    case GLUT_KEY_F2:
        red = 0.0;
        green = 1.0;
        blue = 0.0;
        break;
    case GLUT_KEY_F3:
        red = 0.0;
        green = 0.0;
        blue = 1.0;
        break;
    case GLUT_KEY_UP:
        ycoord += 0.1;
        break;
    case GLUT_KEY_DOWN:
        ycoord -= 0.1;
        break;
    case GLUT_KEY_RIGHT:
        xcoord += 0.1;
        break;
    case GLUT_KEY_LEFT:
        xcoord -= 0.1;
        break;
    case GLUT_KEY_F4:
        angley -= 1;
        break;
    case GLUT_KEY_F5:
        angley += 1;
        break;
    case GLUT_KEY_F6:
        anglex -= 1;
        break;
    case GLUT_KEY_F7:
        anglex += 1;
        break;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(400, 400);
    glutCreateWindow("Test");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);

    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);

    glutMainLoop();

    return 1;
}