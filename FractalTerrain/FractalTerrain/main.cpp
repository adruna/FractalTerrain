
#include "TerrainGenerator.h"
#include "Camera.h"

using namespace std;

static Keyboard keyboard;
static Camera camera;
static TerrainGenerator terrain;

float Globals::lastTotalTime;
float Globals::deltaTime;
float Globals::fixedDeltaTime;
float Globals::lastFixedTime;

float matrix[16];

void getDT()
{
	Globals::deltaTime = glutGet(GLUT_ELAPSED_TIME) - Globals::lastTotalTime;
	Globals::lastTotalTime = Globals::deltaTime + Globals::lastTotalTime;
}

// display every 10 ms.
void checkFixedDT()
{
	Globals::lastFixedTime += Globals::deltaTime;
	if (Globals::lastFixedTime >= 0.10f)
	{
		Globals::fixedDeltaTime = Globals::lastFixedTime;
		Globals::lastFixedTime = 0;

		glutPostRedisplay();
	}
}

// Idle, checks to see if its time to display or not, and tells keyboard to update.
void idle()
{
	getDT();
	checkFixedDT();

	keyboard.update();
}


// Display stuff.
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	camera.applyViewingTransformations();
	
	terrain.drawTerrain();

	glColor3f(1,1,1);
	glTranslatef(0,0,5);
	glutWireTeapot(1);
	glTranslatef(0,0,-10);
	glutWireTeapot(1);

	glFlush();
	glutSwapBuffers();
}

void passiveMouse(int x, int y)
{
	camera.passiveMouseFunc(x,y);

	idle();
}

// Toggle camera capture, and cursor showing.
void toggleCameraLookAt(unsigned char key, KeyState state)
{ 
	camera.captureMouse = !camera.captureMouse; 
	glutSetCursor((!camera.captureMouse) ? GLUT_CURSOR_INHERIT : GLUT_CURSOR_NONE);
}

// run away
void escapePressed(unsigned char key, KeyState state)
{ exit(0); }

// Keyboard callbacks for doing things with the terrain :D
void stepTerrain(unsigned char key, KeyState state)
{ terrain.nextStep(); }
void iterateTerrain(unsigned char key, KeyState state)
{ terrain.nextIteration(); }
void finishTerrain(unsigned char key, KeyState state)
{ terrain.finish(); }
void resetTerrain(unsigned char key, KeyState state)
{ terrain.reset(); }

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 10000.0);     
   
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();

	camera.reshapeFunc(w,h);
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH);

	keyboard.setIdleCallback(idle);
	keyboard.addKeyHandler(escapePressed, VK_ESCAPE, UP);
	keyboard.addKeyHandler(toggleCameraLookAt, 'm', UP);
	keyboard.addKeyHandler(resetTerrain, 'r', UP);
	keyboard.addKeyHandler(stepTerrain, 'p', DOWNHELD);
	keyboard.addKeyHandler(stepTerrain, 'P', UP);
	keyboard.addKeyHandler(iterateTerrain, 'o', UP);
	keyboard.addKeyHandler(iterateTerrain, 'O', UP);
	keyboard.addKeyHandler(finishTerrain, 'i', UP);
	keyboard.addKeyHandler(finishTerrain, 'I', UP);

	camera.setKeyboardCallbacks(keyboard);
}

int main(int argc, char** argv)
{
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1028, 720);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	init();

	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(Keyboard::keyDownHandler);
	glutKeyboardUpFunc(Keyboard::keyUpHandler);
	glutPassiveMotionFunc(passiveMouse);
	glutMainLoop();

	return 0;
}