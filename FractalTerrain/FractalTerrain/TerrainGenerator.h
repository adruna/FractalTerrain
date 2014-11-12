#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "Vector.h"
#include "GlobalValues.h"
#include <Windows.h>
#include <math.h>
#include <cstdio>

#include <GL\glew.h>
#include <GL\GL.h>

/*
The fractal terrain generator.
At the moment is just a grid of points being drawn.
No fractals to see here at the moment.
*/
class TerrainGenerator
{
private:
	ShaderProgram *shaderProgram;
	GLuint vaoid, vboid, worldxLoc, projLoc;
	int size, length;

	float *heights;
	float *points;

	void updateBuffers(void);

public:
	TerrainGenerator(ShaderProgram* = 0, int = 2);
	~TerrainGenerator();

	void draw(float *, float *);
	void finish(void);
};

/*
All the fractal calculations are done in here.

The three static consts at the beginning control how things are calculated.

class TerrainGenerator
{
public:
	static const int GRID_SIZE = 1024 + 1;
	static const int CELL_SIZE = 2;
	static const int RANDOMNESS = 20;

	GLuint program;

	TerrainGenerator();
	TerrainGenerator(GLuint);
	~TerrainGenerator();

	void handleCameraPosition(Vector3);
	void drawTerrain();

	void finish(void);
	void nextIteration(void);
	void nextStep(void);
	void reset(void);
	
private:
	float points[GRID_SIZE][GRID_SIZE];
	float morePoints[GRID_SIZE][GRID_SIZE][3];
	GLuint	myBuffer;

	int dStep;
	int cPos[2];
	int cSize;
	float cRand;
	int it;
	
	bool finished;
	bool onSquareStep;

	void nextBlock(void);

	void square(void);
	void diamond(void);
	
	void drawNextPoints(void);

	void updateVao(void);
};
*/

#endif