#include "Camera.h"
#include <GL\glfw3.h>

Camera *Camera::current;

/* Constructor! */
Camera::Camera()
{
	current = this;

	forward.z = 1;
	position.y = 0;
	sphericalAngles[0] = 0;
	sphericalAngles[1] = (float)PI;
	boost(); // Make sure we have some speed.

	updateVectors();
}


/* Uses glulookat to orient camera based on position/forward/up. */
void Camera::applyViewingTransformations()
{
	gluLookAt(
		position.x, position.y, position.z,
		position.x + forward.x, position.y + forward.y, position.z + forward.z,
		up.x, up.y, up.z
	);
}

/* Recalculates forward, right, and up orientation vectors based on the spherical coordinates. */
void Camera::updateVectors()
{
	forward.x = cos(sphericalAngles[1]) * sin(sphericalAngles[0]);
	forward.y =	sin(sphericalAngles[1]);
	forward.z =	cos(sphericalAngles[1]) * cos(sphericalAngles[0]);

	right.x = sin(sphericalAngles[0] - (float)PI_2);
	right.y = 0;
	right.z = cos(sphericalAngles[0] - (float)PI_2);

	up = Vector3::Cross(forward, right);
}

/* Handles mouse input for changing the camera's direction. */
void Camera::mouseMoved(int x, int y)
{
	if (captureMouse)
	{
		if (x == center[0] && y == center[1])
			return;

		int dx = (int)center[0] - x;
		int dy = y - (int)center[1];

		sphericalAngles[0] += dx / 250.0f;
		sphericalAngles[1] += dy / 250.0f;

		updateVectors();
	}
}

/* Sets the centers for the mouse calculations. */
void Camera::reshapeFunc(int x, int y)
{
	center.x = (float)(x >> 1);
	center.y = (float)(y >> 1);
}

/* Initilizes camera usage. Applies some keyboard handlers that then call functions in Camera::current. */
void Camera::init()
{
	addKeyHandler(moveForwardCallback, 'W', DOWNHELD);
	addKeyHandler(moveBackCallback, 'S', DOWNHELD);
	addKeyHandler(moveRightCallback, 'D', DOWNHELD);
	addKeyHandler(moveLeftCallback, 'A', DOWNHELD);

	addKeyHandler(moveUpCallback, GLFW_KEY_SPACE, DOWNHELD);
	addKeyHandler(moveUpCallback, GLFW_KEY_LEFT_CONTROL, DOWNHELD); // Dosnt work

	// Also not working.
	addKeyHandler(boostCallback, GLFW_KEY_LEFT_SHIFT, UP);
	addKeyHandler(slowCallback, GLFW_KEY_RIGHT_SHIFT, UP);
}

#pragma region Keyboard Callbacks

/* Keyboard Callback: moves the current camera forward. */
void Camera::moveForwardCallback(int key, KeyState state)
{ current->moveForward(); }

/* Keyboard Callback: moves the current camera backwards. */
void Camera::moveBackCallback(int key, KeyState state)
{ current->moveBack(); }

/* Keyboard Callback: moves the current camera left. */
void Camera::moveLeftCallback(int key, KeyState state)
{ current->moveLeft(); }

/* Keyboard Callback: moves the current camera right. */
void Camera::moveRightCallback(int key, KeyState state)
{ current->moveRight(); }

/* Keyboard Callback: moves the current camera up. */
void Camera::moveUpCallback(int key, KeyState state)
{ current->moveUp(); }

/* Keyboard Callback: moves the current camera down. */
void Camera::moveDownCallback(int key, KeyState state)
{ current->moveDown(); }

/* Keyboard Callback: boosts the camera movement. */
void Camera::boostCallback(int key, KeyState)
{ current->boost(); }

/* Keyboard Callback: slows the camera movement. */
void Camera::slowCallback(int key, KeyState)
{ current->slow(); }

// The actual member functions, THEY DONT USE DT ANYMORE!!(bad)
/* Moves the camera along its forward vector. */
void Camera::moveForward()
{ position += forward * (speed); }

/* Moves the camera along its forward vector a negative amount. */
void Camera::moveBack()
{ position -= forward * (speed); }

/* Moves the camera along its right vector. */
void Camera::moveRight()
{ position -= right * (speed); }

/* Moves the camera along its right vector a negative amount. */
void Camera::moveLeft()
{ position += right * (speed); }

/* Moves the camera along its up vector. */
void Camera::moveUp()
{ position += up * (speed); }

/* Moves the camera along its up vector a negative amount. */
void Camera::moveDown()
{ position -= up * (speed); }

/* Increases the current speed of the camera by 5. */
void Camera::boost()
{ speed += 5; }

/* Decreases the current speed of the camera by 5, if its not already less than 5. */
void Camera::slow()
{ if (speed > 5) speed -= 5; }

#pragma endregion

