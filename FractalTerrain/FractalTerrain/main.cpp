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
#define TERRAIN_EXPONENT 8

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

int windowWidth_i, windowHeight_i;
float windowWidth_f, windowHeight_f;

float cameraMovement[3];
float cameraSpeed = 0;

/*
Draw scene.
*/
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float cameraSmoothing = cameraSpeed * dt;
	
	camera->move(cameraMovement[0] * cameraSmoothing, cameraMovement[1] * cameraSmoothing, cameraMovement[2] * cameraSmoothing);
	//printf("%f %f %f \n", cameraMovement[0] * cameraSmoothing, cameraMovement[1] * cameraSmoothing, cameraMovement[2] * cameraSmoothing);
	camera->applyViewingTransformations();

	// Most expensive call here, but at the frame rate we're at its not worth trying to fix.
	//glGetFloatv(GL_MODELVIEW_MATRIX, world);

	terrain->draw(camera->viewMatrix, proj);
	
	glfwSwapBuffers(window);

	cameraMovement[0] = 0;
	cameraMovement[1] = 0;
	cameraMovement[2] = 0;
}

/*
Recalculate viewport and perspective on window resizing.
Update camera.
*/
void reshape(GLFWwindow *window, int width, int height)
{
	windowWidth_i = width;
	windowHeight_i = height;
	windowWidth_f = (float)width;
	windowHeight_f = (float)height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, windowWidth_f / windowHeight_f, 1.0, 10000.0);
   
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	glGetFloatv(GL_PROJECTION_MATRIX, proj);
}

#pragma region Camera Callbacks
bool captureMouse;

// This was silly, just use seperate functions?
static const int boostPositive = 'R';
static const int boostNegative = 'F';

static const int rightPositive = 'A';
static const int rightNegative = 'D';

static const int upPositive = 'Q';
static const int upNegative = 'E';

static const int forwardPositive = 'W';
static const int forwardNegative = 'S';

// Boost camera speed (slow down if negative boost key).
void cameraBoost(int key, KeyState state)
{
	// negatives are allowed.
	cameraSpeed += 10 * (2 * (key == boostPositive) - 1);
}

// Move camera right (left if negative key).
void moveRight(int key, KeyState state)
{
	cameraMovement[0] = 2 * (key == rightPositive) - 1;
}

// Move camera up (down if negative key).
void moveUp(int key, KeyState state)
{
	cameraMovement[1] = 2 * (key == upPositive) - 1;
}

// Move camera forward (back if negative key).
void moveForward(int key, KeyState state)
{
	cameraMovement[2] = 2 * (key == forwardPositive) - 1;
}

/*
Update the camera when the mouse is moved.
*/
void mouseCallback(GLFWwindow *window, double x, double y)
{
	camera->mouseMoved((float)x, (float)y);
}

/*
Set the previous mouse data to the current mouse data and set the callback back to the normal callback function.
*/
void resetMouseCallback(GLFWwindow *window, double x, double y)
{
	camera->setPreviousMouseData((float)x, (float)y);
	glfwSetCursorPosCallback(window, mouseCallback);
}

/*
If the mouse just left set back to the reset function.
First mouse call will then reset the previous data and then call the normal callback function.
*/
void cursorEnterHandler(GLFWwindow *window, int entered)
{
	if (!entered)
	{
		glfwSetCursorPosCallback(window, resetMouseCallback);
	}
}

/*
Toggle the capture of the mouse / camera lookat functionality.
*/
void toggleCameraLookAt(int key, KeyState state)
{
	float halfWidth = windowWidth_f / 2.0f;
	float halfHeight = windowHeight_f / 2.0f;
	captureMouse = !captureMouse;

	// disabled = 0x34003, hidden = 0x34002, normal = 34001, -> normal + bool*2 => { disabled if true, normal if false }. 
	// hidden still allows the cursor to move around and leave the window, disabled is like 'locking.'
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + captureMouse * 2); 

	glfwSetCursorPos(window, halfWidth, halfHeight);
	camera->setPreviousMouseData(halfWidth, halfHeight);
}

#pragma endregion

#pragma region Terrain Callbacks

/*
Do one iteration of the terrain calculations.
*/
void iterate(int no, KeyState dontcare)
{ 
	terrain->iterate(); 
}

/*
Complete terrain calculations.
*/
void finish(int no, KeyState dontcare)
{ 
	terrain->finish(); 
}

/*
Reset terrain to 0 (with corners).
*/
void reset(int no, KeyState dontcare)
{ 
	terrain->reset(); 
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
	glfwSetCursorPosCallback(window, resetMouseCallback);
	glfwSetWindowSizeCallback(window, reshape);
	glfwSetKeyCallback(window, keyboardGLFWHandler);
	glfwSetCursorEnterCallback(window, cursorEnterHandler);

	// Add keyboard callbacks.
	addKeyHandler(toggleCameraLookAt, 'M', UP);
	addKeyHandler(finish, 'F', UP);
	addKeyHandler(iterate, 'I', UP);
	addKeyHandler(reset, 'R', UP);

	addKeyHandler(moveForward, forwardPositive, DOWNHELD);
	addKeyHandler(moveForward, forwardNegative, DOWNHELD);
	addKeyHandler(moveUp, upPositive, DOWNHELD);
	addKeyHandler(moveUp, upNegative, DOWNHELD);
	addKeyHandler(moveRight, rightPositive, DOWNHELD);
	addKeyHandler(moveRight, rightNegative, DOWNHELD);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Create the shader program and use it. (USING IT is causing some error). // hi brian
	shaderProgram = new ShaderProgram("vertex.glsl", "fragment.glsl");
	glUseProgram(shaderProgram->programID);

	// Move camera up a little so we can actually see the plane.
	// Negative because cameras a bit broken right now.
	camera = new Camera(&dt);
	camera->setPosition(0.0f, -2.0f, 0.0f);
	
	terrain = new TerrainGenerator(shaderProgram, TERRAIN_EXPONENT);
	
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	reshape(window, width, height);

	cameraSpeed = 10;
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
	// UNCOMMENT to ignore opengl stuff and just run the algorithm/reset a ton of times.
	/*
	terrain = new TerrainGenerator(TERRAIN_EXPONENT);

	for (size_t i = 0; i < 50; i++)
	{
		finish(0, DOWN);
		reset(0, DOWN);
	}

	delete terrain;

	return 0;
	//*/

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
		keyboardUpdate();
		glfwPollEvents();
	}
	
	cleanup();

	return 0;
}