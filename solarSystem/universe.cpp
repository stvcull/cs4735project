#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>

#include "constants.h"
#include "universe.h"

#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 800

using namespace solarSystem;
void init();
void initProjection();
void initLight();
void createUniverse();
void display();
void idle();
void drawBody(CelestialBody);
void drawOrbit(CelestialBody);
void lookAt(CelestialBody, double);
Point getPos(CelestialBody);

CelestialBody * sun;
double t;

int main(int argc, char **argv) {
	std::cout << "hi" << std::endl;
	createUniverse();
	glutInit(&argc, argv);
	init();

	glutDisplayFunc(display);
	glutIdleFunc(idle);

	initProjection();
	initLight();

	glutMainLoop();
	return 0;
}

void init() {
	glutInitDisplayMode(GLUT_DOUBLE| GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Universe");
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void initProjection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(4.0, 1.0, 0.1, 1000 * AU);
	gluLookAt(3*AU, 0.0, 0.05, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void initLight() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_LIGHT0);
	GLfloat diffColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffColor);
	GLfloat pos[] = { 800.0, 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
}

void createUniverse() {
	t = 0.0;
	sun = new Planet("sun", SUN_RADIUS, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
			0.0);
	CelestialBody * mercury = new Planet("mercury", MERCURY_RADIUS*50,
	MERCURY_MAJOR, MERCURY_E, MERCURY_APH, MERCURY_PER,
	MERCURY_OMEGA, MERCURY_P, 1.0, 1.0, 1.0);
	CelestialBody * venus = new Planet("venus", VENUS_RADIUS*50,
	VENUS_MAJOR, VENUS_E, VENUS_APH, VENUS_PER, VENUS_OMEGA, VENUS_P, 1.0, 1.0,
			0.4);
	CelestialBody * earth = new Planet("earth", EARTH_RADIUS*50,
	EARTH_MAJOR, EARTH_E, EARTH_APH, EARTH_PER, EARTH_OMEGA, EARTH_P, 0.0, 0.0,
			1.0);
	CelestialBody * moon = new Planet("moon", MOON_RADIUS*50,
	MOON_MAJOR, MOON_E, MOON_APH, MOON_PER, MOON_OMEGA, MOON_P, 0.7, 0.7, 0.7);
	earth->addChild(*moon);

	CelestialBody * mars = new Planet("mars", MARS_RADIUS*50,
	MARS_MAJOR, MARS_E, MARS_APH, MARS_PER, MARS_OMEGA, MARS_P, 1.0, 0.2, 0.2);
	CelestialBody * jupiter = new Planet("jupiter", JUPITER_RADIUS,
	JUPITER_MAJOR, JUPITER_E, JUPITER_APH, JUPITER_PER,
	JUPITER_OMEGA, JUPITER_P, 1.0, 0.5, 0.25);

	sun->addChild(*mercury);
	sun->addChild(*venus);
	sun->addChild(*earth);
	sun->addChild(*mars);
	sun->addChild(*jupiter);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	drawBody(*sun);
	glFlush();
	glutSwapBuffers();
}

void idle() {
	t += 10;
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void drawBody(CelestialBody p) {
	GLUquadricObj* sph = gluNewQuadric();
	gluQuadricDrawStyle(sph, GLU_FILL);
	glPushMatrix();
	//drawOrbit(p);
	glColor3f(p.getRed(), p.getGreen(), p.getBlue());
	double pt = t / p.getPeriod();
	double x = p.getCenter().x + (p.getMajor() * cos(pt * M_PI / 180.0) * cos(
			p.getOmega() * M_PI / 180.0)) - (p.getMinor() * sin(pt * M_PI
			/ 180.0) * sin(p.getOmega() * M_PI / 180.0));
	double y = p.getCenter().y + (p.getMajor() * cos(pt * M_PI / 180.0) * sin(
			p.getOmega() * M_PI / 180.0)) + (p.getMinor() * sin(pt * M_PI
			/ 180.0) * cos(p.getOmega() * M_PI / 180.0));
	glTranslated(x, y, 0);
	gluSphere(sph, p.getRadius(), 10, 10);
	std::cout << p.getName() << " " <<x <<" " <<y << std::endl;
	if (int c = p.hasChildren()) {
		for (int i = 0; i < c; i++) {
			drawBody(p.getChild(i));
		}
	}
	glPopMatrix();
}
void drawOrbit(CelestialBody p) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	double x, y;
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for (double i = 0; i <= 360.0; i += 0.001) {
		x = p.getMajor() * cos((M_PI / 180.0) * i);
		y = p.getMinor() * sin((M_PI / 180.0) * i);
		glVertex3d(x, y, 0);
	}
	glEnd();
	glPopMatrix();
}