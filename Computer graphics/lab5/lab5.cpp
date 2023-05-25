#include <GL/glut.h>
#include <vector>
#include <iostream>
using namespace std;
struct Point
{
    int x, y;
    Point() { y = x = 0; }
    Point(int _x, int _y) : y(_y), x(_x) {}
};
struct Vertex
{
    int x, y;
    char type;
    bool visited, entryType;
    Vertex()
    {
        y = x = 0;
        type = 'v';
        visited = 0;
        entryType = 1;
    }
    Vertex(int _x, int _y, char _type, bool _entryType = 1) : y(_y), x(_x), type(_type), entryType(_entryType) { visited = 0; }
    friend std::ostream &operator<<(std::ostream &out, const Vertex &v);
};
std::ostream &operator<<(std::ostream &out, const Vertex &v)
{
    out << "Vertex(" << v.x << ", " << v.y << ", " << v.type << ", " << v.entryType << ")";
    return out;
}
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
vector<Point> poly(0);
vector<Point> clip(0);
vector<Point> newPoly(0);
int minx = WIDTH, miny = HEIGHT, maxx = 0, maxy = 0;
int polyColor = 1, clipColor = 2;
vector<Vertex> polyList(0);
vector<Vertex> clipList(0);
int intersectionCount = 0;
bool entryOrder = 1;
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
void fill(int x, int y, int c)
{
    switch (c)
    {
    case 0:
        setPixel(x, y, Color(255, 255, 255), PixelBuffer);
        break;
    case 1:
        setPixel(x, y, Color(1, 1, 1), PixelBuffer);
        break;
    case 2:
        setPixel(x, y, Color(1, 1, 255), PixelBuffer);
        break;
    case 3:
        setPixel(x, y, Color(255, 1, 1), PixelBuffer);
        break;
    }
    color[y][x] = c;
}
void clear()
{
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            fill(x, y, 0);
        }
    }
}
bool eq(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) < 4 && abs(y1 - y2) < 4;
}
void Brezenhem(int x0, int y0, int x1, int y1, int c, vector<Vertex> &list, int colorToFind)
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
    fill(x0, y0, c);
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
            if (color[y][x] == colorToFind || color[y - 1][x] == colorToFind && color[y][x - 1] == colorToFind || color[y + 1][x] == colorToFind && color[y][x + 1] == colorToFind || color[y - 1][x] == colorToFind && color[y][x + 1] == colorToFind || color[y + 1][x] == colorToFind && color[y][x - 1] == colorToFind)
            {
                if (!eq(x, y, list[list.size() - 1].x, list[list.size() - 1].y))
                {
                    list.push_back(Vertex(x, y, 'i', entryOrder));
                    entryOrder = !entryOrder;
                }
            }
            fill(x, y, c);
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
            if (color[y][x] == colorToFind || color[y - 1][x] == colorToFind && color[y][x - 1] == colorToFind || color[y + 1][x] == colorToFind && color[y][x + 1] == colorToFind || color[y - 1][x] == colorToFind && color[y][x + 1] == colorToFind || color[y + 1][x] == colorToFind && color[y][x - 1] == colorToFind)
            {
                if (!eq(x, y, list[list.size() - 1].x, list[list.size() - 1].y))
                {
                    list.push_back(Vertex(x, y, 'i', entryOrder));
                    entryOrder = !entryOrder;
                }
            }
            fill(x, y, c);
        } while (x != x1 || y != y1);
    }
}
void Brezenhem(int x0, int y0, int x1, int y1, int c)
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
    fill(x0, y0, c);
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
            fill(x, y, c);
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
            fill(x, y, c);
        } while (x != x1 || y != y1);
    }
}
void setOrder(vector<Point> points, int borderColor)
{
    int x1 = points[0].x, x2 = points[0].x;
    while (x1 != 0 && color[points[0].y][x1 - 1] != borderColor)
    {
        x1--;
    }
    while (x2 != WIDTH && color[points[0].y][x2 + 1] != borderColor)
    {
        x2++;
    }
    if (x1 != 0 && x2 != WIDTH)
        entryOrder = 0;
    else
        entryOrder = 1;
}
void draw(vector<Point> v, int c, vector<Vertex> &list, int colorToFind, bool dir = 1)
{
    list.clear();
    if (dir)
    {
        if (v.size() > 1)
        {
            for (int i = 0; i < v.size() - 1; i++)
            {
                list.push_back(Vertex(v[i].x, v[i].y, 'v'));
                Brezenhem(v[i].x, v[i].y, v[i + 1].x, v[i + 1].y, c, list, colorToFind);
            }
            if (v.size() > 2)
            {
                list.push_back(Vertex(v[v.size() - 1].x, v[v.size() - 1].y, 'v'));
                Brezenhem(v[v.size() - 1].x, v[v.size() - 1].y, v[0].x, v[0].y, c, list, colorToFind);
            }
        }
    }
    else
    {
        if (v.size() > 1)
        {
            for (int i = v.size() - 1; i > 0; i--)
            {
                list.push_back(Vertex(v[i].x, v[i].y, 'v'));
                Brezenhem(v[i].x, v[i].y, v[i - 1].x, v[i - 1].y, c, list, colorToFind);
            }
            if (v.size() > 2)
            {
                list.push_back(Vertex(v[0].x, v[0].y, 'v'));
                Brezenhem(v[0].x, v[0].y, v[v.size() - 1].x, v[v.size() - 1].y, c, list, colorToFind);
            }
        }
    }
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
                poly.push_back(Point(x, y));
                minx = min(minx, x);
                miny = min(miny, y);
                maxx = max(maxx, x);
                maxy = max(maxy, y);
                clear();
                if (poly.size() == 1)
                {
                    fill(x, y, polyColor);
                }
                draw(poly, polyColor, polyList, clipColor);
                draw(clip, clipColor, clipList, polyColor);
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
                clip.push_back(Point(x, y));
                minx = min(minx, x);
                miny = min(miny, y);
                maxx = max(maxx, x);
                maxy = max(maxy, y);
                clear();
                if (clip.size() == 1)
                {
                    fill(x, y, clipColor);
                }
                draw(clip, clipColor, clipList, polyColor);
                draw(poly, polyColor, polyList, clipColor);
            }
        }
    }
    glutPostRedisplay();
}
void clipFunc(bool polyType, int cmpx, int cmpy, int fx, int fy)
{
    vector<Vertex> temp = polyList;
    if (polyType)
        temp = polyList;
    else
        temp = clipList;
    int ind = 0;
    for (int i = 0; i < temp.size(); i++)
    {
        if (eq(temp[i].x, temp[i].y, cmpx, cmpy))
        {
            polyType == 0 ? clipList[i].visited = 1 : polyList[i].visited = 1;
            ind = i;
            break;
        }
    }
    for (int i = ind + 1; i != ind; i++)
    {
        i %= temp.size();
        polyType == 0 ? clipList[i].visited = 1 : polyList[i].visited = 1;
        newPoly.push_back(Point(temp[i].x, temp[i].y));
        if (!polyType && eq(temp[i].x, temp[i].y, fx, fy))
        {
            for (int j = 0; j < newPoly.size() - 1; j++)
            {
                Brezenhem(newPoly[j].x, newPoly[j].y, newPoly[j + 1].x, newPoly[j + 1].y, 3);
            }
            for (int j = newPoly.size() - 1; j > 0; j--)
            {
                Brezenhem(newPoly[j].x, newPoly[j].y, newPoly[j - 1].x, newPoly[j - 1].y, 3);
            }
            newPoly.clear();
            return;
        }
        if (temp[i].type == 'i')
        {
            clipFunc(!polyType, temp[i].x, temp[i].y, fx, fy);
            break;
        }
    }
    return;
}
void clippingAlgorithm(bool entryType)
{
    int fx = 0, fy = 0, fi = -1;
    bool f = 0;
    int i = 0;
    while (i != fi)
    {
        i %= polyList.size();
        if (f)
        {
            polyList[i].visited = 1;
            newPoly.push_back(Point(polyList[i].x, polyList[i].y));
            if (polyList[i].type == 'i')
            {
                clipFunc(0, polyList[i].x, polyList[i].y, fx, fy);
                f = 0;
                continue;
            }
        }
        if (!f && polyList[i].type == 'i' && !polyList[i].visited && polyList[i].entryType == entryType)
        {
            polyList[i].visited = 1;
            newPoly.push_back(Point(polyList[i].x, polyList[i].y));
            fx = polyList[i].x;
            fy = polyList[i].y;
            f = 1;
            if (fi == -1)
                fi = i;
        }
        i++;
    }
}
void keyboardKeys(unsigned char key, int x, int y)
{
    switch (key)
    {
    case ('c'):
        poly.clear();
        clip.clear();
        clear();
        break;
    case ('q'):
        clear();
        draw(clip, clipColor, clipList, polyColor);
        setOrder(poly, clipColor);
        draw(poly, polyColor, polyList, clipColor);
        setOrder(clip, polyColor);
        draw(clip, clipColor, clipList, polyColor);
        if (polyList.size() - poly.size() > 0 && clipList.size() - clip.size() > 0)
            clippingAlgorithm(1);
        break;
    case ('w'):
        clear();
        draw(clip, clipColor, clipList, polyColor);
        setOrder(poly, clipColor);
        draw(poly, polyColor, polyList, clipColor);
        setOrder(clip, polyColor);
        draw(clip, clipColor, clipList, polyColor);
        if (polyList.size() - poly.size() > 0 && clipList.size() - clip.size() > 0)
            clippingAlgorithm(0);
        break;
    case ('e'):
        clear();
        draw(clip, clipColor, clipList, polyColor, 0);
        setOrder(poly, clipColor);
        draw(poly, polyColor, polyList, clipColor);
        setOrder(clip, polyColor);
        draw(clip, clipColor, clipList, polyColor, 0);
        if (polyList.size() - poly.size() > 0 && clipList.size() - clip.size() > 0)
            clippingAlgorithm(0);
        break;
    case ('r'):
        clear();
        draw(clip, clipColor, clipList, polyColor, 0);
        setOrder(poly, clipColor);
        draw(poly, polyColor, polyList, clipColor);
        setOrder(clip, polyColor);
        draw(clip, clipColor, clipList, polyColor, 0);
        if (polyList.size() - poly.size() > 0 && clipList.size() - clip.size() > 0)
            clippingAlgorithm(1);
        break;
    default:
        break;
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
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowPosition(350, 350);
    glutInitWindowSize(WIDTH + 100, HEIGHT + 100);
    glutCreateWindow("GLUT");
    Init();
    glutDisplayFunc(display);
    glutMouseFunc(mouseButton);
    glutKeyboardFunc(keyboardKeys);
    glutReshapeFunc(Reshape);
    glutMainLoop();
    return 0;
}