#include <Windows.h>
#include <cstdio>
#include <GL\glew.h>

#include "TerrainGenerator.h"
#include "Camera.h"
#include "KeyboardHandler.h"

/*
Terrain is defined by a square array of dimension (2^n + 1)^2
This value will be passed to the terrain generator and used as the n value. 
(1-> 3x3, 2-> 5x5, 10-> 1025x1025)
Note, 12 seems to be the highest we can fit into an array at the moment, (make points array [size][3]?).
*/
#define TERRAIN_EXPONENT 7

using namespace std;

Camera *camera;
TerrainGenerator *terrain;
GLFWwindow *window;
ShaderProgram* shaderProgram;

float world[16];
float proj[16];

float *w;
float *p;

float dt;
double currentTime;

float windowWidth, windowHeight;

/*
Draw scene.
*/
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glGetFloatv(GL_MODELVIEW_MATRIX, world);
	
	glLoadIdentity();
	camera->applyViewingTransformations();
	terrain->draw(world, proj);
	
	glfwSwapBuffers(window);
	keyboardUpdate();
}

/*
Recalculate viewport and perspective on window resizing.
Update camera.
*/
void reshape(GLFWwindow *window, int width, int height)
{
	windowWidth = (float)width;
	windowHeight = (float)height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, windowWidth / windowHeight, 1.0, 10000.0);
   
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	camera->reshapeFunc(windowWidth, windowHeight);
	glGetFloatv(GL_PROJECTION_MATRIX, proj);
}

#pragma region Camera Callbacks
// Move these into camera.
/*
Update the camera when the mouse is moved.
*/
void mouseCallback(GLFWwindow *window, double x, double y)
{
	camera->mouseMoved((float)x, (float)y);
	if (camera->captureMouse)
	{ glfwSetCursorPos(window, windowWidth/2.0f, windowHeight/2.0f); }
}

/*
Toggle the capture of the mouse / camera lookat functionality.
*/
void toggleCameraLookAt(int key, KeyState state)
{
	camera->captureMouse = !camera->captureMouse;
	// TODO: hide cursor?
}
#pragma endregion

#pragma region Terrain Callbacks

/*
Do one iteration of the terrain calculations.
*/
void iterate(int no, KeyState dontcare)
{ terrain->iterate(); }

/*
Complete terrain calculations.
*/
void finish(int no, KeyState dontcare)
{ terrain->finish(); }

/*
Reset terrain to 0 (with corners).
*/
void reset(int no, KeyState dontcare)
{ terrain->reset(); }

#pragma endregion

/*
Print out any errors that occur.
*/
void errorCallback(int error , const char* description)
{ fprintf(stderr, description); }


/*
Initialize glfw callbacks, keyboard callbacks.
Create shaders, camera, and terrain.
*/
void init(GLFWwindow *window)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// Setup glfw callbacks.
	glfwSetErrorCallback(errorCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetWindowSizeCallback(window, reshape);
	glfwSetKeyCallback(window, keyboardGLFWHandler);

	// Add keyboard callbacks.
	addKeyHandler(toggleCameraLookAt, 'M', UP);
	addKeyHandler(finish, 'F', UP);
	addKeyHandler(iterate, 'I', UP);
	addKeyHandler(reset, 'R', UP);
	Camera::init();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Create the shader program and use it. (USING IT is causing some error). // hi brian
	shaderProgram = new ShaderProgram("vertex.glsl", "fragment.glsl");
	glUseProgram(shaderProgram->programID);

	camera = new Camera(&dt);
	terrain = new TerrainGenerator(shaderProgram, TERRAIN_EXPONENT);
	
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	reshape(window, width, height);
}

/*
Free up resources and terminate open gl.
*/
void cleanup(void)
{
	delete camera;
	delete terrain;
	delete shaderProgram;

	glfwTerminate();
}

/*
Program entry, window initialization and main loop.
*/
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

	// Main loop.
	while (!glfwWindowShouldClose(window))
	{
		double now = glfwGetTime();
		dt = (float)(now - currentTime);
		currentTime = now;

		display();
		glfwPollEvents();
	}
	
	// Closing
	cleanup();

	return 0;
}


#pragma region Terrain Step-Step callbacks
/*

//put these in init (make them statics in terrain generator too).
addKeyHandler(resetTerrain, 'R', UP);
addKeyHandler(stepTerrain, 'P', UP);
addKeyHandler(iterateTerrain, 'O', UP);
addKeyHandler(finishTerrain, 'I', UP);

// Keyboard callbacks for doing things with the terrain :D
void stepTerrain(int key, KeyState state)
{ }//terrain->nextStep(); }
void iterateTerrain(int key, KeyState state)
{ }//terrain->nextIteration(); }
void finishTerrain(int key, KeyState state)
{ }//terrain->finish(); }
void resetTerrain(int key, KeyState state)
{ }//terrain->reset(); }
*/
#pragma endregion