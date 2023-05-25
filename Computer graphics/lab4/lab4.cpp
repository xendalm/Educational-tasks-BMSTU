#include <GL/glut.h>
#include <vector>
#include <stack>
using namespace std;

struct Line
{
    int y, x;
    Line() { y = x = 0; }
    Line(int _y, int _x) : y(_y), x(_x) {}
};
struct Color
{
    int rgb[3];
    Color(int r, int g, int b)
    {
        rgb[0] = r;
        rgb[1] = g;
        rgb[2] = b;
    }
};
int WIDTH = 400, HEIGHT = 400, xPos, yPos, lastH, lastW;

GLubyte *PixelBuffer = new GLubyte[WIDTH * HEIGHT * 3];
GLubyte *PixelBuffer1 = new GLubyte[WIDTH * HEIGHT * 3];
int **color = new int *[HEIGHT];
vector<pair<double, double>> v(0);
stack<Line> s;
int minx = WIDTH, miny = HEIGHT, maxx = 0, maxy = 0;

void setPixel(int x, int y, Color c, GLubyte *pixels)
{
    int position = (x + y * HEIGHT) * 3;
    pixels[position] = c.rgb[0];
    pixels[position + 1] = c.rgb[1];
    pixels[position + 2] = c.rgb[2];
}
Color getPixel(int x, int y, GLubyte *pixels)
{
    int position = (x + y * HEIGHT) * 3;
    return (Color(pixels[position], pixels[position + 1], pixels[position + 2]));
}

void clear()
{
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            setPixel(x, y, Color(255, 255, 255), PixelBuffer);
            color[y][x] = 0;
        }
    }
}
void fill(int x, int y, int c)
{
    switch (c)
    {
    case 1:
        setPixel(x, y, Color(1, 1, 1), PixelBuffer);
        break;
    case 2:
        setPixel(x, y, Color(135, 206, 235), PixelBuffer);
        break;
    }
    color[y][x] = c;
}
void fillline(int y, int x1, int x2)
{
    for (int i = x1; i <= x2; i++)
    {
        setPixel(i, y, Color(135, 206, 235), PixelBuffer);
        color[y][i] = 2;
    }
}
void Brezenhem(int x0, int y0, int x1, int y1)
{
    int A, B, sign;
    A = y1 - y0;
    B = x0 - x1;
    if (abs(A) > abs(B))
        sign = 1;
    else
        sign = -1;
    int signa, signb;
    if (A < 0)
        signa = -1;
    else
        signa = 1;
    if (B < 0)
        signb = -1;
    else
        signb = 1;
    int f = 0;
    fill(x0, y0, 1);
    int x = x0, y = y0;
    if (sign == -1)
    {
        do
        {
            f += A * signa;
            if (f > 0)
            {
                f -= B * signb;
                y += signa;
            }
            x -= signb;
            fill(x, y, 1);
        } while (x != x1 || y != y1);
    }
    else
    {
        do
        {
            f += B * signb;
            if (f > 0)
            {
                f -= A * signa;
                x -= signb;
            }
            y += signa;
            fill(x, y, 1);
        } while (x != x1 || y != y1);
    }
}
void polyfill(int x, int y)
{
    s.push(Line(y, x));
    while (!s.empty())
    {
        Line l = s.top();
        s.pop();

        int x1 = l.x;
        while (x1 != 0 && color[l.y][x1 - 1] != 1)
        {
            x1--;
        }
        int x2 = l.x;
        while (x2 != WIDTH && color[l.y][x2 + 1] != 1)
        {
            x2++;
        }
        if (x1 != 0 && x2 != WIDTH)
        {
            fillline(l.y, x1, x2);

            bool f1 = 1, f2 = 1;
            for (int i = x1; i <= x2; i++)
            {
                if (color[l.y + 1][i] == 0 && f1)
                {
                    s.push(Line(l.y + 1, i));
                    f1 = 0;
                }
                if (color[l.y + 1][i] == 1)
                    f1 = 1;

                if (color[l.y - 1][i] == 0 && f2)
                {
                    s.push(Line(l.y - 1, i));
                    f2 = 0;
                }
                if (color[l.y - 1][i] == 1)
                    f2 = 1;
            }
        }
    }
}
void avrg1(int i, int j)
{
    int nrgb[3];
    for (int k = 0; k < 3; k++)
    {
        unsigned int av = 0;
        int w[9] = {4, 2, 1};
        av += w[0] * getPixel(j, i, PixelBuffer).rgb[k] + w[1] * getPixel(j + 1, i, PixelBuffer).rgb[k] + w[1] * getPixel(j - 1, i, PixelBuffer).rgb[k] + w[1] * getPixel(j, i + 1, PixelBuffer).rgb[k] + w[1] * getPixel(j, i - 1, PixelBuffer).rgb[k] + w[2] * getPixel(j + 1, i + 1, PixelBuffer).rgb[k] + w[2] * getPixel(j - 1, i - 1, PixelBuffer).rgb[k] + w[2] * getPixel(j + 1, i - 1, PixelBuffer).rgb[k] + w[2] * getPixel(j - 1, i + 1, PixelBuffer).rgb[k];
        nrgb[k] = round(double(av) / (w[0] + 4 * (w[1] + w[2])));
    }
    setPixel(j, i, Color(nrgb[0], nrgb[1], nrgb[2]), PixelBuffer1);
}
void filter1()
{
    memcpy(&PixelBuffer1, &PixelBuffer, sizeof(PixelBuffer1));
    for (int i = miny; i <= maxy; i++)
        for (int j = minx; j <= maxx; j++)
            avrg1(i, j);
    memcpy(&PixelBuffer, &PixelBuffer1, sizeof(PixelBuffer));
}
void avrg(int i, int j, int n)
{
    int nrgb[3];
    for (int k = 0; k < 3; k++)
    {
        unsigned int s = 0;
        for (int k1 = 0; k1 < n; k1++)
            for (int k2 = 0; k2 < n; k2++)
                s += getPixel(j + k2, i + k1, PixelBuffer).rgb[k];
        nrgb[k] = round(double(s) / double(n * n));
    }
    for (int k1 = 0; k1 < n; k1++)
        for (int k2 = 0; k2 < n; k2++)
            setPixel(j + k2, i + k1, Color(nrgb[0], nrgb[1], nrgb[2]), PixelBuffer1);
}
void filter(int n)
{
    memcpy(&PixelBuffer1, &PixelBuffer, sizeof(PixelBuffer1));
    for (int i = miny; i <= miny + (maxy - miny + (maxy - miny) % n) / n; i++)
        for (int j = minx; j <= minx + (maxx - minx + (maxx - minx) % n) / n; j++)
            avrg(miny + (i - miny) * n, minx + (j - minx) * n, n);
    memcpy(&PixelBuffer, &PixelBuffer1, sizeof(PixelBuffer));
}
void keyboardKeys(unsigned char key, int x, int y)
{
    switch (key)
    {
    case ('c'):
        v.clear();
        clear();
        break;
    case ('f'):
        filter(3);
        break;
    case ('g'):
        filter1();
        break;
    default:
        break;
    }
    glutPostRedisplay();
}
void Reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-w / 2., w / 2., -h / 2., h / 2.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void Init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < HEIGHT; ++i)
    {
        color[i] = new int[WIDTH];
    }
    clear();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    xPos = -HEIGHT / 2;
    yPos = -WIDTH / 2;
}
void mouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            if (-yPos > abs(lastH / 2 - y) && -xPos > abs(lastW / 2 - x))
            {
                y = (HEIGHT + lastH) / 2 - y;
                x = x - (lastW - WIDTH) / 2;
                v.push_back(make_pair(x, y));
                minx = min(minx, x);
                miny = min(miny, y);
                maxx = max(maxx, x);
                maxy = max(maxy, y);
                clear();
                if (v.size() == 1)
                {
                    fill(x, y, 1);
                }

                if (v.size() > 1)
                {
                    for (int i = 0; i < v.size() - 1; i++)
                    {
                        Brezenhem(v[i].first, v[i].second, v[i + 1].first, v[i + 1].second);
                    }
                    if (v.size() > 2)
                    {
                        Brezenhem(v[v.size() - 1].first, v[v.size() - 1].second, v[0].first, v[0].second);
                    }
                }
            }
        }
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            if (-yPos > abs(lastH / 2 - y) && -xPos > abs(lastW / 2 - x))
            {
                y = (HEIGHT + lastH) / 2 - y;
                x = x - (lastW - WIDTH) / 2;
                polyfill(x, y);
            }
        }
    }
    glutPostRedisplay();
}
void display()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glPushAttrib(GL_CURRENT_BIT);
    double x, y, z;
    GLdouble modelMatrix[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    GLdouble projMatrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluUnProject(viewport[0] + 1, viewport[1] + 1, 0, modelMatrix, projMatrix, viewport, &x, &y, &z);
    lastH = -y * 2 + 2;
    lastW = -x * 2 + 2;
    int dx, dy;
    dx = dy = 0;
    int xp, yp;
    xp = xPos;
    yp = yPos;
    if (yPos < y)
    {
        dy = y - yPos;
        yp = y;
    }
    if (xPos < x)
    {
        dx = x - xPos;
        xp = x;
    }
    glRasterPos2f(xp + x - trunc(x), yp + y - trunc(y));
    glPixelStorei(GL_UNPACK_ROW_LENGTH, WIDTH);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, dy);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, dx);
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, PixelBuffer);
    glPopAttrib();
    glFlush();
    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("GLUT");
    Init();
    glutDisplayFunc(display);
    glutMouseFunc(mouseButton);
    glutKeyboardFunc(keyboardKeys);
    glutReshapeFunc(Reshape);
    glutMainLoop();
    return 0;
}