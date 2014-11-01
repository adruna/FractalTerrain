#include "Camera.h"

float Camera::speed = 1;
Vector3 Camera::forward, Camera::right, Camera::up, Camera::position;
Vector2 Camera::sphericalAngles, Camera::center;

Camera::Camera()
{
	forward.z = 1;
	position.y = 0;
	sphericalAngles[0] = 0;
	sphericalAngles[1] = PI;

	updateVectors();
}

Camera::~Camera() { }

// Keyboard callbacks for movement things.
void Camera::moveForward(unsigned char key, KeyState state)
{ position += forward * (speed); }

void Camera::moveBack(unsigned char key, KeyState state)
{ position -= forward * (speed); }

void Camera::moveLeft(unsigned char key, KeyState state)
{ position += right * (speed); }

void Camera::moveRight(unsigned char key, KeyState state)
{ position -= right * (speed); }

void Camera::moveUp(unsigned char key, KeyState state)
{ position += up * (speed); }

void Camera::moveDown(unsigned char key, KeyState state) // Never called
{ position -= up * (speed); }

void Camera::boost(unsigned char key, KeyState)
{ speed += 10; }
void Camera::slow(unsigned char key, KeyState)
{ if (speed > 0) speed -= 10; }

// Uses glulookat to orient camera.
void Camera::applyViewingTransformations()
{
	/*
	gluLookAt(
		0, 20, 0,
		1, -2, 0,
		0.5, 0, 0
		);
	/*/
	gluLookAt(
		position.x, position.y, position.z,
		position.x + forward.x, position.y + forward.y, position.z + forward.z,
		up.x, up.y, up.z
	);
	//*/
}

// recalculates forward, right, and up orientation vectors.
void Camera::updateVectors()
{
	forward.x = cos(sphericalAngles[1]) * sin(sphericalAngles[0]);
	forward.y =	sin(sphericalAngles[1]);
	forward.z =	cos(sphericalAngles[1]) * cos(sphericalAngles[0]);

	right.x = sin(sphericalAngles[0] - PI_2);
	right.y = 0;
	right.z = cos(sphericalAngles[0] - PI_2);

	up = Vector3::Cross(forward, right);
}

// handles mouse input.
void Camera::mouseMoved(int x, int y)
{
	if (captureMouse)
	{
		if (x == center[0] && y == center[1])
			return;

		int dx = center[0] - x;
		int dy = y - center[1];

		sphericalAngles[0] += dx / 250.0f;
		sphericalAngles[1] += dy / 250.0f;

		updateVectors();
	}
}

// sets the centers for the mouse calculations.
void Camera::reshapeFunc(int x, int y)
{
	center.x = x >> 1;
	center.y = y >> 1;
}

// yay keyboard callbacks.
void Camera::setKeyboardCallbacks(Keyboard& keyboard)
{
	keyboard.addKeyHandler(moveForward, 'w', DOWNHELD);
	keyboard.addKeyHandler(moveForward, 'W', DOWNHELD);
	keyboard.addKeyHandler(moveBack, 's', DOWNHELD);
	keyboard.addKeyHandler(moveBack, 'S', DOWNHELD);
	keyboard.addKeyHandler(moveRight, 'd', DOWNHELD);
	keyboard.addKeyHandler(moveRight, 'D', DOWNHELD);
	keyboard.addKeyHandler(moveLeft, 'a', DOWNHELD);
	keyboard.addKeyHandler(moveLeft, 'A', DOWNHELD);

	keyboard.addKeyHandler(moveUp, VK_SPACE, DOWNHELD);
	keyboard.addKeyHandler(moveUp, VK_LCONTROL, DOWNHELD); // Dosnt work.

	keyboard.addKeyHandler(boost, 'b', UP);
	keyboard.addKeyHandler(slow, 'B', UP);
}

