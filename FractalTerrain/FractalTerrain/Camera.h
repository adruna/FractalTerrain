#ifndef CAMERA_H
#define CAMERA_H

#include "Keyboard.h"
#include "Vector.h"

/*
This class is used to control where the "camera" is.

Modeled after some generic fps functionality.

Future work
	Implement vectors a bit better.
	Play with input.
	Create an actual matrix instead of using glu lookat.
*/
class Camera
{
private:
	static Camera *current;
	
	bool boosted;
	float *dt;
	float speed;
	Vector2 sphericalAngles, center;
	Vector3 forward, up, right, position;

	void moveForward();
	void moveBack();
	void moveRight();
	void moveLeft();
	void moveUp();
	void moveDown();
	void boost();
	void slow();

	void updateVectors(void);

	// Static keyboard callbacks used by current.
	static void moveForwardCallback(int, KeyState);
	static void moveBackCallback(int, KeyState);
	static void moveRightCallback(int, KeyState);
	static void moveLeftCallback(int, KeyState);
	static void moveUpCallback(int, KeyState);
	static void moveDownCallback(int, KeyState);
	static void boostCallback(int key, KeyState state);
	static void slowCallback(int key, KeyState state);

public:
	bool captureMouse;

	Camera(float *);

	void reshapeFunc(float x, float y);
	void mouseMoved(float x, float y);
	void applyViewingTransformations();

	static void init();
};

#endif