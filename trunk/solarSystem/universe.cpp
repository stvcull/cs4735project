#include "constants.h"
#include "universe.h"

using namespace solarSystem;
void mainLoop();
void init();
void initTextures();
void initDisplay(int&, char**);
bool keyDown(SDL_keysym*);
void keyUp(SDL_keysym*);
void mouseMotion(SDL_MouseMotionEvent*);
void reshape(int, int);
void display();
double findE(int, double, double, double);
void createUniverse();
void createTest();

void drawBody(CelestialBody);

CelestialBody * sun;
double t;
double t_factor;

double aspect = 1.0;

Camera camera;
SDL_Surface* surface;
int videoFlags;
bool forward, backward, left, right, up, down, cw, ccw, zin, zout;
bool mouseCap;
bool lock = true;
int planet = 3;

GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat yellow[] = { 1.0, 1.0, 0.55, 1.0 };
GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };

double findE(int n, double E, double M, double ecc) {

	double newE = E - ((E - ecc * sin(E) - M) / (1 - ecc * cos(E)));

	if (n == 0)
		return E;
	else
		return findE(n - 1, newE, M, ecc);

}

double findM(double per, double t) {
	return (2 * M_PI * t) / per;
}

double findTheta(double e, double E) {

	return 2 * atan(sqrt((1 + e) / (1 - e)) * tan(E / 2));

}

double findP(double a, double ecc) {
	return a * (1 - ecc) * (1 - ecc);
}

double findDist(double p, double ecc, double theta) {
	return p / (1 + ecc * cos(theta));
}

int main(int argc, char **argv) {
	initDisplay(argc, argv);
	init();
	initTextures();

	//createUniverse();
	createTest();

	mainLoop();
	return 0;
}

void initDisplay(int& argc, char** argv) {
	/* this holds some info about our display */
	const SDL_VideoInfo *videoInfo;

	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_Quit();
		exit(1);
	}

	/* Fetch the video info */
	videoInfo = SDL_GetVideoInfo();
	if (!videoInfo) {
		SDL_Quit();
		exit(1);
	}

	/* the flags to pass to SDL_SetVideoMode */
	videoFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE
			| SDL_RESIZABLE;

	/* This checks to see if surfaces can be stored in memory */
	if (videoInfo->hw_available)
		videoFlags |= SDL_HWSURFACE;
	else
		videoFlags |= SDL_SWSURFACE;

	/* This checks if hardware blits can be done */
	if (videoInfo->blit_hw)
		videoFlags |= SDL_HWACCEL;

	/* Sets up OpenGL double buffering */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	/* get a SDL surface */
	surface = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BPP, videoFlags);

	/* Verify there is a surface */
	if (!surface) {
		SDL_Quit();
		exit(1);
	}

	reshape(WINDOW_WIDTH, WINDOW_HEIGHT);

	SDL_WM_SetCaption("Solar System Sim", "Solar System Sim");

	mouseCap = true;
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_ShowCursor(0);
}

void init() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_NORMALIZE);

	GLfloat pos[] = { 0.0, 0.0, 0.0 };

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_AMBIENT, black);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	Point eye(450, 0, 0);
	Point lookAt(0, 0, 0);
	Vector up(0, 1, 0);

	camera.set(eye, lookAt, up, 90);
}

void initTextures() {
	Image* texture = new Image();
	texture->readFile("./textures/sun.bmp");
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
	texture->getWidth(), texture->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
			texture->getData());

	texture->readFile("./textures/mercury.bmp");
	glBindTexture(GL_TEXTURE_2D, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
	texture->getWidth(), texture->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
			texture->getData());

	texture->readFile("./textures/venus.bmp");
	glBindTexture(GL_TEXTURE_2D, 2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
	texture->getWidth(), texture->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
			texture->getData());

	texture->readFile("./textures/earth.bmp");
	glBindTexture(GL_TEXTURE_2D, 3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
	texture->getWidth(), texture->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
			texture->getData());

	texture->readFile("./textures/moon.bmp");
	glBindTexture(GL_TEXTURE_2D, 31);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
	texture->getWidth(), texture->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
			texture->getData());

	texture->readFile("./textures/mars.bmp");
	glBindTexture(GL_TEXTURE_2D, 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
	texture->getWidth(), texture->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
			texture->getData());

	texture->readFile("./textures/jupiter.bmp");
	glBindTexture(GL_TEXTURE_2D, 5);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
	texture->getWidth(), texture->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
			texture->getData());

	texture->readFile("./textures/saturn.bmp");
	glBindTexture(GL_TEXTURE_2D, 6);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
	texture->getWidth(), texture->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
			texture->getData());

	texture->readFile("./textures/uranus.bmp");
	glBindTexture(GL_TEXTURE_2D, 7);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
	texture->getWidth(), texture->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
			texture->getData());

	texture->readFile("./textures/neptune.bmp");
	glBindTexture(GL_TEXTURE_2D, 8);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
	texture->getWidth(), texture->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
			texture->getData());

	texture->readFile("./textures/pluto.bmp");
	glBindTexture(GL_TEXTURE_2D, 9);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
	texture->getWidth(), texture->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
			texture->getData());
}

void mainLoop() {
	bool loop = true;
	SDL_Event event;
	forward = backward = left = right = up = down = cw = ccw = zin = zout
			= false;
	while (loop) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_VIDEORESIZE:
				surface = SDL_SetVideoMode(event.resize.w, event.resize.h, BPP,
				videoFlags);
				if (!surface)
					loop = false;
				reshape(event.resize.w, event.resize.h);
				break;
			case SDL_KEYDOWN:
				loop = keyDown(&event.key.keysym);
				break;
			case SDL_KEYUP:
				keyUp(&event.key.keysym);
				break;
			case SDL_MOUSEMOTION:
				if (mouseCap)
					mouseMotion(&event.motion);
				break;
			case SDL_QUIT:
				loop = false;
				break;
			}
		}

		double du = 0, dv = 0, dn = 0;
		if (forward)
			dn -= MOVE_SPEED;
		if (backward)
			dn += MOVE_SPEED;
		if (left)
			du -= MOVE_SPEED;
		if (right)
			du += MOVE_SPEED;
		if (ccw)
			camera.roll(-MOVE_SPEED);
		if (cw)
			camera.roll(MOVE_SPEED);
		if (up)
			dv += MOVE_SPEED;
		if (down)
			dv -= MOVE_SPEED;
		if (zin)
			camera.zoom(-ZOOM_SPEED);
		if (zout)
			camera.zoom(ZOOM_SPEED);

		camera.translate(du, dv, dn);

		t += t_factor;

		display();
		SDL_GL_SwapBuffers();
	}

	SDL_Quit();
}

bool keyDown(SDL_keysym *keysym) {
	switch (keysym->sym) {
	case SDLK_F1:
		SDL_WM_ToggleFullScreen(surface);
		return true;
	case SDLK_w:
		return forward = true;
	case SDLK_s:
		return backward = true;
	case SDLK_a:
		return left = true;
	case SDLK_d:
		return right = true;
	case SDLK_q:
		return ccw = true;
	case SDLK_e:
		return cw = true;
	case SDLK_r:
		return zin = true;
	case SDLK_f:
		return zout = true;
	case SDLK_SPACE:
		return up = true;
	case SDLK_LCTRL:
		return down = true;
	case SDLK_ESCAPE:
		return false;
	case SDLK_TAB:
		mouseCap = !mouseCap;
		if (mouseCap) {
			SDL_WM_GrabInput(SDL_GRAB_ON);
			SDL_ShowCursor(0);
		} else {
			SDL_WM_GrabInput(SDL_GRAB_OFF);
			SDL_ShowCursor(1);
		}
		break;
	case SDLK_p:
		t_factor *= 10.0;
		break;
	case SDLK_MINUS:
		t_factor *= 0.1;
		break;
	default:
		return true;
	}

	return true;
}

void keyUp(SDL_keysym *keysym) {
	switch (keysym->sym) {
	case SDLK_w:
		forward = false;
		break;
	case SDLK_s:
		backward = false;
		break;
	case SDLK_a:
		left = false;
		break;
	case SDLK_d:
		right = false;
		break;
	case SDLK_q:
		ccw = false;
		break;
	case SDLK_e:
		cw = false;
		break;
	case SDLK_r:
		zin = false;
		break;
	case SDLK_f:
		zout = false;
		break;
	case SDLK_SPACE:
		up = false;
		break;
	case SDLK_LCTRL:
		down = false;
		break;
	default:
		break;
	}
}

void mouseMotion(SDL_MouseMotionEvent *m) {
	camera.pitch((double) m->yrel * MOUSE_SENSITIVITY);
	camera.yaw((double) m->xrel * MOUSE_SENSITIVITY);
}

void reshape(int x, int y) {
	aspect = (double) x / (double) y;
	glViewport(0, 0, x, y);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	if (!lock) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(camera.getFOV(), aspect, 0.01, 5000.0);
		Point pos = camera.getEye();
		Point dir = moveAlong(pos, invert(camera.getN()));
		Vector up = camera.getV();
		gluLookAt(pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, up.x, up.y, up.z);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawBody(*sun);
	glFlush();
}

void createTest() {
	t = 0.0;
	t_factor = 0.00001;

	sun = new CelestialBody(SUN_ID, SUN_R, SUN_ROT_PER, SUN_ATILT,
			SUN_SEMI_MAJOR, SUN_ECC, SUN_PER, SUN_OM, SUN_INC, SUN_W);
	CelestialBody * child1 = new CelestialBody(MERCURY_ID, MERCURY_R,
			MERCURY_ROT_PER, MERCURY_ATILT, MERCURY_SEMI_MAJOR, MERCURY_ECC,
			MERCURY_PER, MERCURY_OM, MERCURY_INC, MERCURY_W);
	CelestialBody * child2 = new CelestialBody(VENUS_ID, VENUS_R,
			VENUS_ROT_PER, VENUS_ATILT, VENUS_SEMI_MAJOR, VENUS_ECC,
			VENUS_PER, VENUS_OM, VENUS_INC, VENUS_W);
	CelestialBody * child3 = new CelestialBody(EARTH_ID, EARTH_R,
			EARTH_ROT_PER, EARTH_ATILT, EARTH_SEMI_MAJOR, EARTH_ECC,
			EARTH_PER, EARTH_OM, EARTH_INC, EARTH_W);
	CelestialBody * child31 = new CelestialBody(MOON_ID, MOON_R, MOON_ROT_PER,
	MOON_ATILT, MOON_SEMI_MAJOR, MOON_ECC, MOON_PER, MOON_OM, MOON_INC, MOON_W);

	child3->addChild(*child31);

	CelestialBody * child4 = new CelestialBody(MARS_ID, MARS_R, MARS_ROT_PER,
	MARS_ATILT, MARS_SEMI_MAJOR, MARS_ECC, MARS_PER, MARS_OM, MARS_INC, MARS_W);
	CelestialBody * child5 = new CelestialBody(JUPITER_ID, JUPITER_R,
			JUPITER_ROT_PER, JUPITER_ATILT, JUPITER_SEMI_MAJOR, JUPITER_ECC,
			JUPITER_PER, JUPITER_OM, JUPITER_INC, JUPITER_W);
	CelestialBody * child6 = new CelestialBody(SATURN_ID, SATURN_R,
			SATURN_ROT_PER, SATURN_ATILT, SATURN_SEMI_MAJOR, SATURN_ECC,
			SATURN_PER, SATURN_OM, SATURN_INC, SATURN_W);
	CelestialBody * child7 = new CelestialBody(URANUS_ID, URANUS_R,
			URANUS_ROT_PER, URANUS_ATILT, URANUS_SEMI_MAJOR, URANUS_ECC,
			URANUS_PER, URANUS_OM, URANUS_INC, URANUS_W);
	CelestialBody * child8 = new CelestialBody(NEPTUNE_ID, NEPTUNE_R,
			NEPTUNE_ROT_PER, NEPTUNE_ATILT, NEPTUNE_SEMI_MAJOR, NEPTUNE_ECC,
			NEPTUNE_PER, NEPTUNE_OM, NEPTUNE_INC, NEPTUNE_W);
	CelestialBody * child9 = new CelestialBody(PLUTO_ID, PLUTO_R,
			PLUTO_ROT_PER, PLUTO_ATILT, PLUTO_SEMI_MAJOR, PLUTO_ECC,
			PLUTO_PER, PLUTO_OM, PLUTO_INC, PLUTO_W);

	sun->addChild(*child1);
	sun->addChild(*child2);
	sun->addChild(*child3);
	sun->addChild(*child4);
	sun->addChild(*child5);
	sun->addChild(*child6);
	sun->addChild(*child7);
	sun->addChild(*child8);
	sun->addChild(*child9);
}

void drawBody(CelestialBody p) {
	glPushMatrix();

	double x = 0;
	double z = 0;

	GLUquadricObj * sph = gluNewQuadric();
	gluQuadricNormals(sph, GLU_SMOOTH);
	gluQuadricTexture(sph, GL_TRUE);

	glBindTexture(GL_TEXTURE_2D, p.getId());

	if (p.getId() == 0)
		glMaterialfv(GL_FRONT, GL_EMISSION, yellow);
	else {
		glMaterialfv(GL_FRONT, GL_EMISSION, black);

		double a = p.getSemiMajor();
		double ecc = p.getEcc();
		double per = p.getPeriod();
		double lan = p.getOmega(); //Longitude of Ascending Node
		double apa = p.getW(); //Argument of Periapsis
		double i = p.getInc(); //inclination

		double slr = findP(a, ecc); //semi-latus rectum
		double M = findM(per, t); //Mean Anomoly
		double E = findE(NEWTON_DEPTH, NEWTON_GUESS, M, ecc);
		double theta = findTheta(ecc, E);
		double dist = findDist(slr, ecc, theta);

		x = dist * sin(theta);
		z = dist * -cos(theta);

		glRotated(apa, 0, 1, 0);//w
		glRotated(i, 1, 0, 0);//i
		glRotated(lan, 0, 1, 0);//omega
		glTranslated(x, 0, z);

		if (lock && p.getId() == planet) {
			Point ppos(x + 2 * p.getRadius(), 0, z);
			Point plook(x, 0, z);
			Vector pup(0, 1, 0);
			camera.set(ppos, plook, pup, camera.getFOV());
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(camera.getFOV(), aspect, 0.01, 5000.0);
			Point pos = camera.getEye();
			Point dir = moveAlong(pos, invert(camera.getN()));
			Vector up = camera.getV();
			glRotated(apa, 0, 1, 0);//w
			glRotated(i, 1, 0, 0);//i
			glRotated(lan, 0, 1, 0);//omega
			gluLookAt(pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, up.x, up.y,
					up.z);

			glMatrixMode(GL_MODELVIEW);
		}

		glRotated(-lan, 0, 1, 0);//omega
		glRotated(-i, 1, 0, 0);//i
		glRotated(-apa, 0, 1, 0);//w

		glPushMatrix();

	}

	glRotated(360 * t / p.getRotPer(), 0, 1, 0); //rotation as a function of time!
	glRotated(90 + p.getATilt(), -1, 0, 0);//TODO Axial Tilt
	gluSphere(sph, p.getRadius(), 100, 100);
	glPopMatrix();

	if (p.getId() == 0)
		glMaterialfv(GL_FRONT, GL_EMISSION, yellow);

	if (int c = p.hasChildren()) {
		for (int i = 0; i < c; i++) {
			drawBody(p.getChild(i));
		}
	}

	glPopMatrix();
}
