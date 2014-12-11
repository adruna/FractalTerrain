#include <ppl.h> // Need to include ppl before thread, so include this before TG.h which has to include thread.
#include "TerrainGenerator.h"

#include <Windows.h>
#include <math.h>
#include <cstdio>
#include <time.h>

#include <GL\glew.h>
#include <GL\GL.h>

// Convert to float once.
static const float RANDF_MAX = (float)RAND_MAX;

#pragma region Helpers

/*
Takes the vbo to update, the array of points to give it, and the heights to update those points with.
Also need size and length of the heights array.
I might take binding buffer data out of these later.
*/
void updateVertexBuffers(GLuint vboid, float *points, float *heights, int size, int length)
{
	float halfLength = length / 2.0f;

	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < length; j++)
		{
			int v = (i*length + j) * 4;

			points[v] = i - halfLength;
			points[v + 1] = heights[i*length + j];
			points[v + 2] = j - halfLength;
			points[v + 3] = 1;
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, vboid);
	glBufferData(GL_ARRAY_BUFFER, size*sizeof(float) * 4, points, GL_STATIC_DRAW);
}

/*
Calculates the index buffer array for a the terrain.
Need a pointer to the array to store into, length of a side, and number of indices in the array.
I might take binding buffer data out of these later.
*/
void initIndexBuffer(GLuint iboid, int numIndicies, GLuint *indexData, unsigned int length)
{
	// Uncomment if need everything to start at zero.
	//memset(indexData, 0, indices*sizeof(GLuint));

	int index = 0;
	int vert = 0;

	unsigned int iLength = length - 1;
	unsigned int jLength = length - 1;

	for (unsigned int i = 0; i < iLength; i++)
	{
		for (unsigned int j = 0; j < jLength; j++)
		{
			indexData[index] = vert;
			indexData[index + 1] = vert + 1;
			indexData[index + 2] = vert + length + 1;

			indexData[index + 3] = vert;
			indexData[index + 4] = vert + length;
			indexData[index + 5] = vert + 1 + length;
			vert += 1;
			index += 6;
		}
		vert += 1;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndicies*sizeof(GLuint), indexData, GL_STATIC_DRAW);
}

/*
Populates the rands array with random numbers.
Needs a seed (time.h's time(nullptr) works).
(This is run on a seperate thread).
*/
void randf(int seed, float *rands, unsigned int size)
{
	for (size_t i = 0; i < size; i++)
	{
		seed *= 0x343FD;
		seed += 0x269EC3;
		rands[i] = (float)((seed >> 0x10) & RAND_MAX) / RANDF_MAX;
	}
}

#pragma endregion

/*
Default constructor that initializes what we need for the algorithm, but not for drawing.
*/
TerrainGenerator::TerrainGenerator(int power)
{
	// 2^(power) vertices on each side of the square. (regretable cast but oh well, only on creation).
	length = (int)pow(2.0f, power) + 1;

	// Total number of vertices
	size = length * length;

	heights = new float[size];
	rands = new float[size];

	resetArrays();
}

/*
Creates the terrain generator and anything needed to draw it (buffers).
Requires a shader to use and the power to use. (2^power + 1) vertices.
*/
TerrainGenerator::TerrainGenerator(ShaderProgram* program, int power) : TerrainGenerator::TerrainGenerator(power)
{
	//TerrainGenerator::TerrainGenerator(power);

	shaderProgram = program;

	// Total number of indices needed to draw
	indices = (length - 1) * (length - 1) * 6;

	// Allocate array data. Indexdata pointer is local because we don't need to keep that around on the cpu.
	
	points = new float[size * 4]; // This can get quite large..
	GLuint *indexData = new GLuint[indices];
	
	// Get world and projection uniforms.
	worldxLoc = glGetUniformLocation(shaderProgram->programID, "world");
	projLoc = glGetUniformLocation(shaderProgram->programID, "proj");

	// Create Vertex Array.
	glGenVertexArrays(1, &vaoid);
	glBindVertexArray(vaoid);

	// Create the Vertex Buffer Object for the Vertex Array.
	// Potentialy chnge this to GL_STATIC_DRAW, if continuing with the visualization stuff.
	glGenBuffers(1, &vboid);
	updateVertexBuffers(vboid, points, heights, size, length);
	// Binding taken care of in updateVertexBuffers

	// Create the Index Buffer for the Vertex Array.
	glGenBuffers(1, &iboid);
	initIndexBuffer(iboid, indices, indexData, length);
	// Binding taken care of in initIndexBuffer
	
	// Specify vertex components and enable.
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Clean up index data because we wont be needing to change this.
	delete indexData;
}

/*
Cleans up buffers and memory.
*/
TerrainGenerator::~TerrainGenerator()
{
	delete[size] heights;
	delete[size] rands;

	if (randThread)
	{
		randThread->detach();
		delete randThread;
	}

	// Don't check in release, this is only here for testing when we dont create points.
#if defined(DEBUG) | defined(_DEBUG)
	if (points != nullptr)
	{
#endif
		delete[size * 4] points;

		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &vboid);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &iboid);

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &vaoid);

#if defined(DEBUG) | defined(_DEBUG)
	}
#endif
}

/*
Sets heights to their intial values and binds buffer.
*/
void TerrainGenerator::resetArrays()
{
	// Calculate all the random numbers we'll need.
	if (randThread) 
	{
		randThread->detach(); 
		delete randThread;
	}
	randThread = new std::thread(randf, (int)time(nullptr), rands, size);

	// Clear to 0
	memset(heights, 0, size*sizeof(float));

	// Set stride.
	stride = length - 1;
	float halfStride = (float)stride / 2.0f;
	
	randIndex = 0;

	// Set initial corner values.
	heights[0] = rands[randIndex++] * (float)stride - halfStride;
	heights[length - 1] = rands[randIndex++] * (float)stride - halfStride;
	heights[size - length] = rands[randIndex++] * (float)stride - halfStride;
	heights[size - 1] = rands[randIndex++] * (float)stride - halfStride;
}

/*
Resets the terrains arrays back to initial values, and if we have a shader sends this data to the buffers.
*/
void TerrainGenerator::reset()
{
	resetArrays();

	// Only here for testing when we don't assign shader and just calculate the heights.
#if defined(DEBUG) | defined(_DEBUG)
	if (shaderProgram)
#endif
		updateVertexBuffers(vboid, points, heights, size, length);
}

/*
Draws the terrain.
Takes a world matrix and projection matrix to use in drawing.
*/
void TerrainGenerator::draw(float *worldMat, float *projMat)
{
	glUniformMatrix4fv(worldxLoc, 1, false, worldMat);
	glUniformMatrix4fv(projLoc, 1, false, projMat);
	
	glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, NULL);

	// Uncomment to draw lines between all the points (makes them as if drawing triangles, so theres some extra lines going between the top/bottom)
	//glDrawElements(GL_LINE_STRIP, indices, GL_UNSIGNED_INT, NULL);

	// Uncomment to draw the individual points.
	//glDrawArrays(GL_POINTS, 0, size);
}

/*
Performs the square/diamond steps for this iteration.
This is where a lot of stuff happens!
*/
void TerrainGenerator::iterate(bool finishing)
{
	// Don't bother if we're already done!!
	if (!(stride-1))
		return;

	unsigned int halfStride = stride >> 1;
	unsigned int halfStrideByLength = halfStride * length;

	// Square
	//for (int x = 0; x < length - 1; x += stride)
	Concurrency::parallel_for(size_t(0), (size_t)(length - 1), stride, [&](size_t x)
	{
		for (int y = 0; y < (int)length - 1; y += stride)
		{
			int topLeft = x*length + y;
			int topRight = topLeft + stride*length;
			int center = topLeft + halfStride*(length + 1);

			float tl = heights[topLeft];			// Top Left.
			float bl = heights[topLeft + stride];	// Bottom Left.
			float tr = heights[topRight];			// Top Right.
			float br = heights[topRight + stride];	// Bottom Right.
			
			// center = avg of square around it + some random.
			heights[center] = (tl + bl + tr + br) / 4.0f + rands[randIndex++] * (float)halfStride - (float)halfStride / 2.0f;
		}
	});

	// Diamond
	//for (int x = 0; x < length; x += halfStride)
	Concurrency::parallel_for(size_t(0), length, (unsigned int)halfStride, [&](size_t x)
	{
		// 0, halfstride, 0, halfstride...
		int offset = ((x / halfStride) & (0x1))*halfStride;

		for (int y = 0; y - offset < (int)(length - 1); y+=stride)
		{
			unsigned int top = x * length + y - offset;
			unsigned int center = top + halfStride;

			float numerator = 0, div = 0;

			// Seems that branching is actualy faster here.
			if (y - offset >= 0)			{ numerator += heights[top]; div++; }
			if (y + offset < (int)length)	{ numerator += heights[top + stride]; div++; }
			if (x != 0)						{ numerator += heights[center - halfStrideByLength]; div++; }
			if (x != length - 1)			{ numerator += heights[center + halfStrideByLength]; div++; }
			
			// See bottom of file for attempt at removing if's.

			// Center = avg diamond around it + some random.
			heights[center] = (numerator) / div + rands[randIndex++] * (float)halfStride - (float)halfStride / 2.0f;
			
		}
	});

	stride = halfStride;

	if (!finishing)
		updateVertexBuffers(vboid, points, heights, size, length);
}

/*
Complete terrain calculations.
Right now just randomizes between 0-5.
*/
void TerrainGenerator::finish()
{
	while (stride - 1)
	{ iterate(true); }

	// Only here for testing when we don't assign shader and just calculate the heights.
#if defined(DEBUG) | defined(_DEBUG)
	if (shaderProgram)
#endif
	// Update, and re-assign buffer data.
	updateVertexBuffers(vboid, points, heights, size, length);
}

// Attempt to remove branching in diamond (turned out to be no faster).
/*
//if (y - offset >= 0)			{ numerator += heights[top]; div++; }
temp = (y - offset) >= 0;
div += temp;
numerator += heights[top * temp] * (float)temp;
//if (y + offset < (int)length)	{ numerator += heights[top + stride]; div++; }
temp = (y + offset) < (int)length;
div += temp;
numerator += heights[(top + stride) * temp] * (float)temp;
//if (x != 0)						{ numerator += heights[center - halfStrideByLength]; div++; }
temp = (x) != 0;
div += temp;
numerator += heights[(center - halfStrideByLength) * temp] * (float)temp;
//if (x != length - 1)			{ numerator += heights[center + halfStrideByLength]; div++; }
temp = (x) != length - 1;
div += temp;
numerator += heights[(center + halfStrideByLength) * temp] * (float)temp;
*/