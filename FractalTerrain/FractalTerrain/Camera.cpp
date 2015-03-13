#include "Camera.h"

#include "GlobalValues.h"
#include <math.h>

/* Set right, up, forward, spherical angles defaults. */
Camera::Camera(float *deltaTime, bool _invertX, bool _invertY)
{
	dt_ptr = deltaTime;

	invertX = _invertX;
	invertY = _invertY;
	
	right[0] = 1;
	up[1] = 1;
	forward[2] = 1;

	updateVectors();
}

/* 
Sets the matrix values, this mignt not be necessary... (have r,u,f vectors just be these mem locations) 
Also, this is kinda wrong anyways but surprisingly it works for right now, this will be the next thing to come back and look at.
I'll also explain whats going on here in more detail when I've fixed it.
*/
void Camera::applyViewingTransformations()
{
	matrix[0] = right[0];
	matrix[1] = up[0];
	matrix[2] = forward[0];
	matrix[3] = 0;

	matrix[4] = right[1];
	matrix[5] = up[1];
	matrix[6] = forward[1];
	matrix[7] = 0;

	matrix[8] = right[2];
	matrix[9] = up[2];
	matrix[10] = forward[2];
	matrix[11] = 0;

	matrix[12] = right[0] * position[0] + right[1] * position[1] + right[2] * position[2];
	matrix[13] = up[0] * position[0] + up[1] * position[1] + up[2] * position[2];
	matrix[14] = forward[0] * position[0] + forward[1] * position[1] + forward[2] * position[2];
	matrix[15] = 1;

	/*
	gluLookAt(
		position[0], position[1], position[2],
		position[0] + forward[0], position[1] + forward[1], position[2] + forward[2],
		up[0], up[1], up[2]
	);
	*/
}

/* Recalculates forward, right, and up orientation vectors based on the spherical coordinates. */
void Camera::updateVectors()
{
	float cosY = cos(sphericalAngles[1]);

	forward[0] = cosY * sin(sphericalAngles[0]);
	forward[1] = sin(sphericalAngles[1]);
	forward[2] = cosY * cos(sphericalAngles[0]);

	right[0] = -sin(sphericalAngles[0] - (float)PI_2);
	right[1] = 0;
	right[2] = -cos(sphericalAngles[0] - (float)PI_2);

	// Up = Forward cross Right. 
	// i   j   k
	// a   b   c
	// A   B   C
	// i(bC - cB) - j (aC - cA) + k (aB - bA)
	up[0] = forward[1] * right[2] - forward[2] * right[1];
	up[1] = forward[2] * right[0] - forward[0] * right[2];
	up[2] = forward[0] * right[1] - forward[1] * right[0];
}

/* Handles mouse input for changing the camera's direction. */
void Camera::mouseMoved(float x, float y)
{	
	float dt = *dt_ptr;
	float dx = x - prevousMouse[0];
	float dy = y - prevousMouse[1];

	sphericalAngles[0] += dx * dt * (2.0f*invertX - 1.0f);
	sphericalAngles[1] += dy * dt * (2.0f*invertY - 1.0f);

	prevousMouse[0] = x;
	prevousMouse[1] = y;

	updateVectors();
}

/* World position set. */
void Camera::setPosition(float x, float y, float z)
{
	position[0] = x;
	position[1] = y;
	position[2] = z;
}

/* position += x * right + y * up + z * forward; */
void Camera::move(float x, float y, float z)
{
	position[0] += (x * right[0] + y * up[0] + z * forward[0]);
	position[1] += (x * right[1] + y * up[1] + z * forward[1]);
	position[2] += (x * right[2] + y * up[2] + z * forward[2]);
}

/* Now the dx,dy will be based off these. */
void Camera::setPreviousMouseData(float x, float y)
{
	prevousMouse[0] = x;
	prevousMouse[1] = y;
}