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

	static void moveForward(unsigned char, KeyState);
	static void moveBack(unsigned char, KeyState);
	static void moveRight(unsigned char, KeyState);
	static void moveLeft(unsigned char, KeyState);
	static void moveUp(unsigned char, KeyState);
	static void moveDown(unsigned char, KeyState);

	static void boost(unsigned char key, KeyState state);
	static void slow(unsigned char key, KeyState state);

	void updateVectors(void);

public:
	static Vector3 forward, up, right, position;
	bool captureMouse;

	Camera();
	~Camera();

	void setKeyboardCallbacks(Keyboard& keyboard);
	void reshapeFunc(int x, int y);
	void mouseMoved(int x, int y);
	void applyViewingTransformations();
};

#endif