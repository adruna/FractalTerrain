#include "GlobalValues.h"
#include <GL\glew.h>
#include <GL\glfw3.h>
#include "TerrainGenerator.h"
#include "Camera.h"

/*
Terrain is defined by a square array of dimension (2^n + 1)^2
This value will be passed to the terrain generator and used as the n value. 
(1-> 3x3, 2-> 5x5, 10-> 1025x1025) 
*/
#define TERRAIN_EXPONENT 9

using namespace std;

Camera *camera;
TerrainGenerator *terrain;
GLFWwindow *window;
ShaderProgram* shaderProgram;

float world[16];
float proj[16];

/*
Draw scene.
*/
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glGetFloatv(GL_MODELVIEW_MATRIX, world);
	glGetFloatv(GL_PROJECTION_MATRIX, proj);
	
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
void reshape(GLFWwindow *window, int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 10000.0);     
   
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	camera->reshapeFunc(w,h);
}

#pragma region Camera Callbacks
// Move these into camera.
/*
Update the camera when the mouse is moved.
*/
void mouseCallback(GLFWwindow *window, double x, double y)
{
	camera->mouseMoved(x, y);
	if (camera->captureMouse)
	{ glfwSetCursorPos(window, 400, 400); }
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
	Camera::init();

	// Create the shader program and use it. (USING IT is causing some error). // hi brian
	shaderProgram = new ShaderProgram("vertex.glsl", "fragment.glsl");
	glUseProgram(shaderProgram->programID);

	camera = new Camera();
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