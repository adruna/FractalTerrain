#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "GlobalValues.h"
#include <thread>

/*
The fractal terrain generator.

Future work:
	Might need more work to allow more than one of these reliably.
	Growable plane?? (or rather subdivide a big plane).
	String 2 of these's sides together (~infinite).

*/

class TerrainGenerator
{
private:
	ShaderProgram *shaderProgram;
	unsigned int vaoid, vboid, iboid, worldxLoc, projLoc, size, length, indices, stride, randIndex;

	float *heights, *rands, *points;
	std::thread *randThread;

	void resetArrays();

public:
	TerrainGenerator(int = 2);
	TerrainGenerator(ShaderProgram*, int = 2);
	~TerrainGenerator();

	void reset(void);
	void draw(float *, float *);
	void iterate(bool = false);
	void finish(void);
};

#endif