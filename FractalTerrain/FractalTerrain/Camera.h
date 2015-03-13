#ifndef CAMERA_H
#define CAMERA_H

#include "Keyboard.h"

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
	float prevousMouse[2];
	float *dt_ptr;

	// Spherical interpretation of look.
	float sphericalAngles[2];

	// Transform.
	float position[3];
	float forward[3];
	float up[3]; 
	float right[3];
	float speed;
	float matrix[16];

	void updateVectors(void);
	
public:
	bool captureMouse;
	bool invertY;
	bool invertX;
	float *viewMatrix = matrix;
	
	/*
	 Constructs a camera.
	 Requires a pointer to the applications delta time.
	 Optional set to invert X or Y, default false and true respectively.
	*/
	Camera(float *dt, bool invertX = false, bool invertY = true);

	/*
	 Modfies the azimuthal and altitudinal angles based on the change in mouse x and y.
	 Then updates the forward/up/right vectors accordingly.
	*/
	void mouseMoved(float mouseX, float mouseY);

	/*
	 Constructs the view matrix.
	*/
	void applyViewingTransformations(void);

	/* 
	 Sets the camera's position. 
	*/
	void setPosition(float x, float y, float z);

	/* 
	 Moves the camera local to it's transform. 
	 X - move to camera's right.
	 Y - move to camera's up.
	 Z - move to camera's forward.
	*/
	void move(float x, float y, float z);

	/* 
	 Boosts the camera's move speed. 
	*/
	void boost(float boostAmount);

	/*
	Sets previous mouse data.
	Use when cursor might suddenly have a new position you don't want affecting the camera.
	*/
	void setPreviousMouseData(float mouseX, float mouseY); 
};

#endif