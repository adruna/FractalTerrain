#ifndef CAMERA_H
#define CAMERA_H

#include "Keyboard.h"
#include "Vector.h"
#include "GlobalValues.h"

#include <math.h>
#include <Windows.h>
#include <GL\GLU.h>

/*
This class is used to control where the "camera" is.

Modeled after some generic fps functionality.
*/
class Camera
{
private:
	static float speed;
	static Vector2 sphericalAngles, center;

	static void moveForward(int, KeyState);
	static void moveBack(int, KeyState);
	static void moveRight(int, KeyState);
	static void moveLeft(int, KeyState);
	static void moveUp(int, KeyState);
	static void moveDown(int, KeyState);

	static void boost(int key, KeyState state);
	static void slow(int key, KeyState state);

	void updateVectors(void);

public:
	static Vector3 forward, up, right, position;
	bool captureMouse;

	Camera();
	~Camera();

	void setKeyboardCallbacks();
	void reshapeFunc(int x, int y);
	void mouseMoved(int x, int y);
	void applyViewingTransformations();
};

#endif