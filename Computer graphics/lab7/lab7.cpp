#include <gl\glut.h>		// ������������ ���� ��� GLu32 ����������
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h";
#define  PI  3.14159265359
#include <iostream>
#include <Windows.h>
#include <ctime>
#include <string>
using namespace std;

struct point {
	float x, y, z;
	point() : x(0), y(0), z(0) {};
	point(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
	friend std::ostream& operator<< (std::ostream& out, const point& p);
} a[10], b[10];
std::ostream& operator<< (std::ostream& out, const point& p) {
	out << "Point(" << p.x << ", " << p.y << ", " << p.z << ")";
	return out;
}
point count_normal(point p1, point p2, point p3) {
	point a, b, n;
	GLfloat l;
	a.x = p2.x - p1.x;
	a.y = p2.y - p1.y;
	a.z = p2.z - p1.z;
	b.x = p3.x - p1.x;
	b.y = p3.y - p1.y;
	b.z = p3.z - p1.z;
	n.x = (a.y * b.z) - (a.z * b.y);
	n.y = (a.z * b.x) - (a.x * b.z);
	n.z = (a.x * b.y) - (a.y * b.x);
	l = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
	n.x /= l;
	n.y /= l;
	n.z /= l;
	return n;
}
GLuint	texture[1];
GLvoid LoadGLTextures() {
	int x, y, n;
	unsigned char* data = stbi_load("3.bmp", &x, &y, &n, 0);
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, x, y, 0,
		n == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}
double rotate_x = 0, rotate_y = 0;
unsigned int n = 9;
float radius = 0;
float zprism = 4;
float light_pos0[] = { 2, 4, 2, 1 };
float light_pos1[] = { 2, -4, 2, 1 };
float eyeX = 6, eyeY = 0, eyeZ = -0.5;
float minz = 1e10;
float zfloor = -2;
float h = 0;
int state = -1;
float g = 0.00002;
float acceleration = g, v = 0;
float vertices1[30], vertices2[30], vertices3[10 * 2 * 3];
float normal1[30], normal2[10 * 2 * 3];
float texcoord1[20], texcoord2[10 * 2 * 2];
GLuint index1[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
GLuint index2[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
void calculate(float r) {
	radius = r;
	float segment = 2 * PI / n;
	for (int i = 0; i < n; i++) {
		float x = r * cos(i * segment);
		float y = r * sin(i * segment);
		a[i].x = b[i].x = x;
		a[i].y = b[i].y = y;
		a[i].z = r;
		b[i].z = -r;
	};
	a[9] = a[0];
	b[9] = b[0];

	float cosa = cos(rotate_x * PI / 180), cosb = cos(rotate_y * PI / 180), sina = sin(rotate_x * PI / 180), sinb = sin(rotate_y * PI / 180);
	for (int i = 0; i < n; i++) {
		minz = min(minz, -sinb * a[i].x + cosb * (sina * a[i].y + cosa * a[i].z));
	}
	for (int i = 0; i < n; i++) {
		minz = min(minz, -sinb * a[i].x + cosb * (sina * a[i].y + cosa * (a[i].z - 2 * radius)));
	}

	for (int i = 0; i < 10; i++) {
		vertices1[3 * i] = a[i].x;
		vertices1[3 * i + 1] = a[i].y;
		vertices1[3 * i + 2] = a[i].z;
		texcoord1[2 * i] = 0.5 + a[i].x / radius * 0.5;
		texcoord1[2 * i + 1] = 0.5 + a[i].y / radius * 0.5;
		normal1[3 * i] = 0;
		normal1[3 * i + 1] = 0;
		normal1[3 * i + 2] = 1;
	}
	for (int i = 0; i < 10; i++) {
		vertices2[3 * i] = b[i].x;
		vertices2[3 * i + 1] = b[i].y;
		vertices2[3 * i + 2] = b[i].z;
	}
	for (int i = 0; i <= n; i++) {
		vertices3[6 * i] = a[i].x; vertices3[6 * i + 1] = a[i].y; vertices3[6 * i + 2] = a[i].z;
		vertices3[6 * i + 3] = b[i].x; vertices3[6 * i + 4] = b[i].y; vertices3[6 * i + 5] = b[i].z;
		if (i % 2 == 0) {
			texcoord2[4 * i] = 0; texcoord2[4 * i + 1] = 0;
			texcoord2[4 * i + 2] = 1; texcoord2[4 * i + 3] = 0;
		}
		else {
			texcoord2[4 * i] = 0; texcoord2[4 * i + 1] = 1 * sin(PI / n);
			texcoord2[4 * i + 2] = 1; texcoord2[4 * i + 3] = 1 * sin(PI / n);
		}

		point normal = count_normal(a[i], b[i], a[i + 1]);
		normal2[6 * i] = normal.x; normal2[6 * i + 1] = normal.y; normal2[6 * i + 2] = normal.z;
		normal2[6 * i + 3] = normal.x; normal2[6 * i + 4] = normal.y; normal2[6 * i + 5] = normal.z;
	}
}
void prism() {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, &texcoord1);
	glVertexPointer(3, GL_FLOAT, 0, &vertices1);
	glNormalPointer(GL_FLOAT, 0, &normal1);
	glDrawElements(GL_POLYGON, 10, GL_UNSIGNED_INT, &index1);


	glTexCoordPointer(2, GL_FLOAT, 0, &texcoord1);
	glVertexPointer(3, GL_FLOAT, 0, &vertices2);
	glNormalPointer(GL_FLOAT, 0, &normal1);
	glDrawElements(GL_POLYGON, 10, GL_UNSIGNED_INT, &index1);


	glTexCoordPointer(2, GL_FLOAT, 0, &texcoord2);
	glVertexPointer(3, GL_FLOAT, 0, &vertices3);
	glNormalPointer(GL_FLOAT, 0, &normal2);
	glDrawElements(GL_QUAD_STRIP, 20, GL_UNSIGNED_INT, &index2);


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
}
void calc() {
	v += acceleration;
	h += v * state;
	if (h < zfloor - minz - zprism) {
		h = zfloor - minz - zprism;
		acceleration *= -1;
		state *= -1;
	}
	if (v < 0) {
		h = 0;
		acceleration *= -1;
		state *= -1;
	}
}
double prevTime = 0.0;
double crntTime = 0.0;
double timeDiff;
unsigned int counter = 0;
double res1 = 0, res2 = 0;
int i = -5;
int dl1, dl2;
void makeList() {
	dl1 = glGenLists(1);
	glNewList(dl1, GL_COMPILE);
	glRotatef(15, 0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	prism();
	glEndList();

	dl2 = glGenLists(1);
	glNewList(dl2, GL_COMPILE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ,
		0, 0, 0,
		0, 0, 1);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
	glColor3d(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
	glVertex3f(-3, -5, zfloor);
	glVertex3f(3, -5, zfloor);
	glVertex3f(3, 5, zfloor);
	glVertex3f(-3, 5, zfloor);
	glEnd();
	glEndList();
}
void display() {
	calc();
	glCallList(dl2);
	glTranslatef(0, 0, zprism + h);
	glRotatef(rotate_x, 1, 0, 0);
	glRotatef(rotate_y, 0, 1, 0);
	glCallList(dl1);

	crntTime = time(NULL);
	timeDiff = crntTime - prevTime;
	counter++;
	if (timeDiff >= 1.0 / 30.0) {
		if (i > 0) {
			res1 += (1.0 / timeDiff) * counter;
			res2 += (timeDiff / counter) * 1000;
		}
		if (i == 10) {
			cout << res1 / 10 << " " << res2 / 10;
			Sleep(3000000);
		}
		i++;
		std::string FPS = std::to_string((1.0 / timeDiff) * counter);
		std::string ms = std::to_string((timeDiff / counter) * 1000);
		std::string Tittle = FPS + "FPS/ " + ms + "ms/";
		glutSetWindowTitle(Tittle.c_str());
		prevTime = crntTime;
		counter = 0;
	}

	glutSwapBuffers();
}
void normalKeyFunction(unsigned char key, int x, int y) {
	switch (key) {
	case 'r':
		h = 0;
		acceleration = g;
		v = 0;
		state = -1;
		break;
	case 'a':
		eyeY -= 0.1;
		if (eyeY < -3) eyeY = -3;
		break;
	case 'd':
		eyeY += 0.1;
		if (eyeY > 3) eyeY = 3;
		break;
	case 's':
		eyeZ -= 0.1;
		if (eyeZ < -3) eyeZ = -3;
		break;
	case 'w':
		eyeZ += 0.1;
		if (eyeZ > 3) eyeZ = 3;
		break;
	}
	glutPostRedisplay();
}
void specialKeyFunction(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		rotate_x -= 15;
		break;
	case GLUT_KEY_RIGHT:
		rotate_x += 15;
		break;
	case GLUT_KEY_DOWN:
		rotate_y += 15;
		break;
	case GLUT_KEY_UP:
		rotate_y -= 15;
		break;
	}
	minz = 1e10;
	float cosa = cos(rotate_x * PI / 180), cosb = cos(rotate_y * PI / 180), sina = sin(rotate_x * PI / 180), sinb = sin(rotate_y * PI / 180);
	for (int i = 0; i < n; i++) {
		minz = min(minz, -sinb * a[i].x + cosb * (sina * a[i].y + cosa * a[i].z));
	}
	for (int i = 0; i < n; i++) {
		minz = min(minz, -sinb * a[i].x + cosb * (sina * a[i].y + cosa * (a[i].z - 2 * radius)));
	}
	glutPostRedisplay();
}
void enableLight() {
	glShadeModel(GL_SMOOTH);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	GLfloat material_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	float diffuse0[] = { 0.2, 0.7, 0.4 };
	float diffuse1[] = { 0.2, 0.3, 0.8 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.5);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.5);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.05);
}
void initGL() {
	LoadGLTextures();
	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	calculate(1);
	makeList();
}
void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, w / h, 1, 100);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	glutCreateWindow("test");
	initGL();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(specialKeyFunction);
	glutKeyboardFunc(normalKeyFunction);
	enableLight();

	glutMainLoop();
	return 0;
}