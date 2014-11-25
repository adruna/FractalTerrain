#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "GlobalValues.h"

/*
The fractal terrain generator.
At the moment is just a grid of points being drawn.
No fractals to see here at the moment.
*/
class TerrainGenerator
{
private:
	ShaderProgram *shaderProgram;
	unsigned int vaoid, vboid, iboid, worldxLoc, projLoc, size, length, indices;
	unsigned int iteration, stride;

	float *heights;
	float *points;

public:
	TerrainGenerator(ShaderProgram* = 0, int = 2);
	~TerrainGenerator();

	void draw(float *, float *);
	void iterate(bool = false);
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