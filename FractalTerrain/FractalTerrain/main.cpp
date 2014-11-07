
#include "GlobalValues.h"
#include <GL\glew.h>
#include <GL\glfw3.h>
#include "TerrainGenerator.h"
#include "Camera.h"


using namespace std;

Camera *camera;
TerrainGenerator *terrain;
GLFWwindow *window;

float matrix[16];

// Display stuff.
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	camera->applyViewingTransformations();
	terrain->drawTerrain();
	
	//glFlush();
	glfwSwapBuffers(window);
	keyboardUpdate();
}


// Toggle camera capture, and cursor showing.
void toggleCameraLookAt(int key, KeyState state)
{ 
	camera->captureMouse = !camera->captureMouse; 
	// TODO: hide cursor?
}

// run away
void escapePressed(int key, KeyState state)
{ exit(0); }

// Keyboard callbacks for doing things with the terrain :D
void stepTerrain(int key, KeyState state)
{ terrain->nextStep(); }
void iterateTerrain(int key, KeyState state)
{ terrain->nextIteration(); }
void finishTerrain(int key, KeyState state)
{ terrain->finish(); }
void resetTerrain(int key, KeyState state)
{ terrain->reset(); }

void reshape(GLFWwindow *window, int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 10000.0);     
   
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	printf("woo");
	camera->reshapeFunc(w,h);
}

void mouseCallback(GLFWwindow *window, double x, double y)
{
	camera->mouseMoved(x, y);
	if (camera->captureMouse)
	{ glfwSetCursorPos(window, 400, 400); }
}

void errorCallback(int a , const char* b)
{
	throw b;
}

void init(GLFWwindow *window)
{
	glfwSetErrorCallback(errorCallback);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH);

	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetWindowSizeCallback(window, reshape);
	glfwSetKeyCallback(window, keyboardGLFWHandler);
	
	addKeyHandler(escapePressed, VK_ESCAPE, UP);
	addKeyHandler(toggleCameraLookAt, 'M', UP);
	addKeyHandler(resetTerrain, 'R', UP);
	addKeyHandler(stepTerrain, 'P', UP);
	addKeyHandler(iterateTerrain, 'O', UP);
	addKeyHandler(finishTerrain, 'I', UP);
	
	Camera::init();

	// Shaders.
	
	//GLuint program = initShader("vertex.glsl", "fragment.glsl");
	//glUseProgram(program);
	camera = new Camera();
	terrain = new TerrainGenerator();
	//terrain->program = program;

	reshape(window, 800, 800);
}

int main(int argc, char** argv)
{
	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(800, 800, "FractalTerrain", nullptr, nullptr);

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
	delete camera;
	delete terrain;
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