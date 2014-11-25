#include "TerrainGenerator.h"

#include <Windows.h>
#include <math.h>
#include <cstdio>

#include <GL\glew.h>
#include <GL\GL.h>

#pragma region Helpers

/*
Takes the vbo to update, the array of points to give it, and the heights to update those points with.
Also need size and length of the heights array.
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
*/
void initIndexBuffer(GLuint iboid, int numIndicies, GLuint *indexData, unsigned int length)
{
	// Uncomment if need everything starting at zero.
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

#pragma endregion

/*
Creates the terrain generator and anything needed to draw it (buffers).
Requires a shader to use and the power to use. (2^power + 1) vertices.
*/
TerrainGenerator::TerrainGenerator(ShaderProgram* program, int power)
{
	shaderProgram = program;
	iteration = 0;

	// 2^(power) vertices on each side of the square. (regretable cast but oh well, only on creation).
	stride = length = (int)pow(2.0f, power) + 1;
	stride -= 1;

	// Total number of vertices
	size = length * length;

	// Total number of indices needed to draw
	indices = (length - 1) * (length - 1) * 6;

	// Allocate array data. Indexdata pointer is local because we don't need to keep that around on the cpu.
	heights = new float[size];
	points = new float[size * 4];
	GLuint *indexData = new GLuint[indices];
	memset(heights, 0, size*sizeof(float));
	heights[0] = 1;
	heights[size-1] = 2;

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
Cleans up buffers.
*/
TerrainGenerator::~TerrainGenerator()
{
	delete[size] heights;
	delete[size*4] points;

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vboid);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &iboid);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vaoid);

}

/*
Draws the terrain.
Takes a world matrix and projection matrix to use in drawing.
*/
void TerrainGenerator::draw(float *worldMat, float *projMat)
{
	glUniformMatrix4fv(worldxLoc, 1, false, worldMat);
	glUniformMatrix4fv(projLoc, 1, false, projMat);
	
	//glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, NULL);
	glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, NULL);

	// Uncomment to draw the individual points.
	//glDrawArrays(GL_POINTS, 0, size);
}

/*
Performs the square/diamond steps for this iteration.
*/
void TerrainGenerator::iterate(bool finishing)
{
	// Don't bother if we're already done!!
	if (!(stride-1))
		return;

	int halfStride = stride >> 1;
	int x = 0, y = 0;

	for (x = 0; x < length - 1; x += stride)
	{
		for (y = 0; y < length - 1; y += stride)
		{
			int topLeft = x*length + y;
			int topRight = topLeft + stride*length;

			float tl = heights[topLeft];			// Top Left.
			float bl = heights[topLeft + stride];	// Bottom Left.
			float tr = heights[topRight];			// Top Right.
			float br = heights[topRight + stride];	// Bottom Right.
			
			heights[topLeft + halfStride*(length + 1)] = (tl + bl + tr + br) / 4.0f;
			//printf("Square : Loc (%i), tl: %f, bl: %f, tr: %f, br: %f\n", topLeft, tl, bl, tr, br);
		}
	}
	
	for (x = 0; x < length; x+=halfStride)
	{
		int offset = ((x / halfStride) & (0x1))*halfStride;
		// 0, halfstride, 0, halfstride.

		for (y = 0; y - offset < (int)(length - 1); y+=stride)
		{
			int top = x * length + y - offset;
			int bottom = top + stride;
			int left = top - halfStride * (int)length + halfStride;
			int right = top + halfStride * (int)length + halfStride;

			// More branching D:
			float t = (y - offset >= 0) ? heights[top] : 0;			// Top.
			float l = (x != 0) ? heights[left] : 0;					// Left.
			float r = (x != length - 1) ? heights[right] : 0;		// Right.
			float b = (y + offset < length) ? heights[bottom] : 0;	// Bottom.


			heights[top + halfStride] = (t + l + r + b) / 4.0f;
			//printf("Diamond : Loc (%i), tl: %f, bl: %f, tr: %f, br: %f\n", top, t, l, r, b);
		}
	}

	stride = halfStride;
	iteration++;

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

	// Update, and re-assign buffer data.
	updateVertexBuffers(vboid, points, heights, size, length);
}

/*

// Sets everything back to 0.
void TerrainGenerator::reset()
{
	updateVao();
	
	// init.
	it = 1;
	cSize = GRID_SIZE >> 1;
	cRand = cSize / it;
	finished = false;
	onSquareStep = true;
	dStep = 0;
	cPos[0] = cSize;
	cPos[1] = cSize;

	// Set four corners to random vals.
	points[0][0] = Globals::randf(-cRand,cRand);
	points[GRID_SIZE-1][0] = Globals::randf(-cRand,cRand);
	points[0][GRID_SIZE-1] = Globals::randf(-cRand,cRand);
	points[GRID_SIZE-1][GRID_SIZE-1] = Globals::randf(-cRand,cRand);
}

// Performs the square step then calls next block.
void TerrainGenerator::square()
{
	points[cPos[0]][cPos[1]] = ((
		points[cPos[0] - cSize][cPos[1] - cSize] +
		points[cPos[0] + cSize][cPos[1] - cSize] +
		points[cPos[0] - cSize][cPos[1] + cSize] +
		points[cPos[0] + cSize][cPos[1] + cSize]
	) / 4.0f ) + Globals::randf(-cRand, cRand);

	nextBlock();
}

// Performs the diamond step, and then increments 'dStep' unless its the last step, then it goes to next block.
void TerrainGenerator::diamond()
{
	int c[2];

	c[0] = cPos[0];
	c[1] = cPos[1];

	switch (dStep)
	{
		case 0: c[0] -= cSize; break; // left
		case 1: c[0] += cSize; break; // right
		case 2: c[1] -= cSize; break; // up
		case 3: c[1] += cSize; break; // down
	}

	float result = 0;
	float div = 0;
	
	if (c[0] - cSize >= 0)			{ result += points[c[0] - cSize][c[1]]; div++; } // left
	if (c[0] + cSize < GRID_SIZE)	{ result += points[c[0] + cSize][c[1]]; div++; } // right
	if (c[1] - cSize >= 0)			{ result += points[c[0]][c[1] - cSize]; div++; } // up
	if (c[1] + cSize < GRID_SIZE)	{ result += points[c[0]][c[1] + cSize]; div++; } // down
	
	result /= div;
	result += Globals::randf(-cRand, cRand);
	points[c[0]][c[1]] = result;

	if (dStep == 3)
	{
		dStep = 0; 
		nextBlock();
	}
	else
		dStep++;
}

// Calls steps untill its time for the next block, 
// if its the last block of an iteration, checks to see if we need to go again for diamond.
// Otherwise halfs size and resets for the next iteration (unless finished of course)
void TerrainGenerator::nextBlock()
{
	if (finished) return;
	
	if (cPos[0] + cSize + 1 < GRID_SIZE) // If not at the end of the row, move to next square in row
		cPos[0] += cSize * 2;
	else if (cPos[1] + cSize + 1 < GRID_SIZE) // If not at the last column, go to the next row.
	{
		cPos[0] = cSize;
		cPos[1] += cSize * 2;
	}
	else if (cSize > 0) // If we're this far, we're in the last position (check to make sure not 0 though).
	{
		if (!onSquareStep) // If still on square we need to go through again and do the diamonds
		{
			// otherwise half the size if not already 1, if so set to 0 so we finish.
			if (cSize == 1) cSize = 0;
			else cSize >>= 1;

			cRand = cSize / (float)it * (float)RANDOMNESS / 5.0f;
			it++;
		}

		onSquareStep = !onSquareStep;
		cPos[0] = cPos[1] = cSize;
	}

	if (cSize == 0) // Then we're done.
		finished = true;
}

// Checks if on square or diamond
void TerrainGenerator::nextStep()
{
	if (onSquareStep)
		square();
	else
		diamond();
}

// While still on (square/diamond) keeps steping
void TerrainGenerator::nextIteration()
{
	bool temp = onSquareStep;
	while (temp == onSquareStep && !finished)
	{ nextStep(); }
}

// While not finished, step
void TerrainGenerator::finish()
{
	while (!finished)
	{ nextStep(); } 
}

// Convience method for the color transitions.
float lerp(float tMax, float tMin, float t, float min, float max)
{
	if (t > tMax) return max;
	if (t < tMin) return min;
	float val = ((t - tMin) / (tMax - tMin)) * max + min;
	if (val > max) return max;
	if (val < min) return min;
	return val;
}

// Draws the grid using the points[][] array as the y values
void TerrainGenerator::drawTerrain()
{
	int halfSize = GRID_SIZE/2;
	float y;

	// Loop through each x, drawing lines along the z axis.
	for (int x = 0; x < GRID_SIZE; x++)
	{
		glBegin(GL_LINE_STRIP);
		for (int z = 0; z < GRID_SIZE; z++)
		{
			y = points[x][z];
			if (y < -300) glColor3f(0.0f, 0.2f, 0.8f); // blue water
			else if	(y > 300) glColor3f(0.8f, 0.8f, 0.8f); // white snow
			else if (y > 100) glColor3f(lerp(250,100,y,0, 0.4f), 0.8 - lerp(300,150,y, 0.0, 0.6f), 0.0); // transition from green to a brown to show "tree line"
			else glColor3f(0.0, 0.8f, 0.2f); // green grass
				
			glVertex3f((x-halfSize) * CELL_SIZE, y, (z-halfSize) * CELL_SIZE);
		}
		glEnd();
	}
	
	// loop through each z, drawing lines along the x axis.
	for (int z = 0; z < GRID_SIZE; z++)
	{
		glBegin(GL_LINE_STRIP);
		for (int x = 0; x < GRID_SIZE; x++)
		{
			y = points[x][z];
			// same colors as above
			if (y < -300) glColor3f(0.0f, 0.2f, 0.8f);
			else if	(y > 300) glColor3f(0.8f, 0.8f, 0.8f);
			else if (y > 100) glColor3f(lerp(250,100,y,0, 0.4f), 0.8 - lerp(300,150,y, 0.0, 0.6f), 0.0);
			else glColor3f(0.0, 0.8f, 0.2f);

			glVertex3f((x-halfSize) * CELL_SIZE, y, (z-halfSize) * CELL_SIZE);
		}
		glEnd();
	}

	glColor3f(1,1,1);

	drawNextPoints();
}

// Draws the next points to be calculated (green for point to be calculated, red for the points used in the averaging.
void TerrainGenerator::drawNextPoints()
{
	if (finished) return; // Dont do this if finished

	int halfSize = GRID_SIZE/2;
	int x = 0,z = 0;

	glPointSize(10.0f);
	glBegin(GL_POINTS);
	
	if (onSquareStep)
	{
		glColor3f(0,1,0);
		x = cPos[0]; z = cPos[1];
		glVertex3f( (x - halfSize) * CELL_SIZE, points[x][z], (z - halfSize) * CELL_SIZE);
		
		glColor3f(1,0,0);
		x = cPos[0] - cSize; z = cPos[1] - cSize; // top left
		glVertex3f( (x - halfSize) * CELL_SIZE, points[x][z], (z - halfSize) * CELL_SIZE);
		x = cPos[0] + cSize; z = cPos[1] - cSize; // top right
		glVertex3f( (x - halfSize) * CELL_SIZE, points[x][z], (z - halfSize) * CELL_SIZE);
		x = cPos[0] - cSize; z = cPos[1] + cSize; // bottom left
		glVertex3f( (x - halfSize) * CELL_SIZE, points[x][z], (z - halfSize) * CELL_SIZE);
		x = cPos[0] + cSize; z = cPos[1] + cSize; // bottom right
		glVertex3f( (x - halfSize) * CELL_SIZE, points[x][z], (z - halfSize) * CELL_SIZE);
	}
	else
	{
		int c[2];
		c[0] = cPos[0];
		c[1] = cPos[1];

		switch (dStep)
		{
			case 0: c[0] -= cSize; break;
			case 1: c[0] += cSize; break;
			case 2: c[1] -= cSize; break;
			case 3: c[1] += cSize; break;
		}

		glColor3f(0,1,0);
		x = c[0]; z = c[1];
		glVertex3f( (x - halfSize) * CELL_SIZE, points[x][z], (z - halfSize) * CELL_SIZE);
		
		glColor3f(1,0,0);

		x = c[0] + cSize; z = c[1]; // right
		if (x < GRID_SIZE)	
			glVertex3f( (x - halfSize) * CELL_SIZE, points[x][z], (z - halfSize) * CELL_SIZE);

		x = c[0] - cSize; // left
		if (x >= 0)			
			glVertex3f( (x - halfSize) * CELL_SIZE, points[x][z], (z - halfSize) * CELL_SIZE);

		x = c[0]; z = c[1] + cSize; // down
		if (z < GRID_SIZE)	
			glVertex3f( (x - halfSize) * CELL_SIZE, points[x][z], (z - halfSize) * CELL_SIZE);

		z = c[1] - cSize; // up
		if (z >= 0)			
			glVertex3f( (x - halfSize) * CELL_SIZE, points[x][z], (z - halfSize) * CELL_SIZE);
	}

	glEnd();
	glPointSize(1);
	glColor3f(1,1,1);
	
}
*/