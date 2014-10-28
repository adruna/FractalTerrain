#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "Vector.h"
#include "GlobalValues.h"
#include <Windows.h>
#include <math.h>
#include <cstdio>

#include <GL/GL.h>

class TerrainGenerator
{
public:
	static const int GRID_SIZE = 1024 + 1;
	static const int CELL_SIZE = 2;
	static const int RANDOMNESS = 20;

	TerrainGenerator();
	~TerrainGenerator();

	void handleCameraPosition(Vector3);
	void drawTerrain();

	void finish(void);
	void nextIteration(void);
	void nextStep(void);
	void reset(void);
	
private:
	float points[GRID_SIZE][GRID_SIZE];

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
};

#endif