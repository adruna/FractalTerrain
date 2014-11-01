
#include "GlobalValues.h"
#include <GL\glew.h>
#include <GL\glfw3.h>
#include "TerrainGenerator.h"
#include "Camera.h"

using namespace std;

static Keyboard keyboard;
static Camera camera;
static TerrainGenerator terrain;

GLFWwindow *window;

float matrix[16];


// Idle, checks to see if its time to display or not, and tells keyboard to update.
void idle()
{
	keyboard.update();
}


// Display stuff.
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();

	camera.applyViewingTransformations();
	terrain.drawTerrain();
	
	//glFlush();
	glfwSwapBuffers(window);

	//glColor3f(1,1,1);
	//glTranslatef(0,0,5);
	//glutWireTeapot(1);
	//glTranslatef(0,0,-10);
	//glutWireTeapot(1);

	//glFlush();
	//glutSwapBuffers();
}


// Toggle camera capture, and cursor showing.
void toggleCameraLookAt(unsigned char key, KeyState state)
{ 
	camera.captureMouse = !camera.captureMouse; 
	//glutSetCursor((!camera.captureMouse) ? GLUT_CURSOR_INHERIT : GLUT_CURSOR_NONE);
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

void reshape(GLFWwindow *window, int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 10000.0);     
   
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	printf("woo");
	camera.reshapeFunc(w,h);
}

void mouseCallback(GLFWwindow *window, double x, double y)
{
	camera.mouseMoved(x, y);
	if (camera.captureMouse)
	{ glfwSetCursorPos(window, 400, 400); }

	printf("%f %f \n", (float)x, (float)y);
}

void init(GLFWwindow *window)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH);

	terrain.nextIteration();
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetWindowSizeCallback(window, reshape);
	glfwSetCharCallback(window, keyboard.keyHandler);

	reshape(window, 800, 800);

	
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
	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(800, 800, "lol", nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewInit();

	init(window);

	while (!glfwWindowShouldClose(window))
	{
		display();
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
/*
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
*/