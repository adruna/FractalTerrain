#include "TerrainGenerator.h"

/*
Creates the terrain generator and anything needed to draw it (buffers).
Requires a shader to use and the power to use. (2^power + 1) vertices.
*/
TerrainGenerator::TerrainGenerator(ShaderProgram* program, int power)
{
	length = pow(2.0f, power) + 1;
	size = length * length;

	heights = new float[size];
	points = new float[size * 4];

	int halfLength = length / 2;

	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < length; j++)
		{
			int v = (i*length + j) * 4;

			points[v] = i - halfLength;
			points[v + 1] = heights[i*length + j] = 0;
			points[v + 2] = j - halfLength;
			points[v + 3] = 1;
		}
	}

	shaderProgram = program;

	worldxLoc = glGetUniformLocation(shaderProgram->programID, "world");
	projLoc = glGetUniformLocation(shaderProgram->programID, "proj");

	glGenVertexArrays(1, &vaoid);
	glBindVertexArray(vaoid);

	glGenBuffers(1, &vboid);
	glBindBuffer(GL_ARRAY_BUFFER, vboid);
	glBufferData(GL_ARRAY_BUFFER, size*sizeof(float)*4 -1, points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

/*
Cleans up buffers.
*/
TerrainGenerator::~TerrainGenerator()
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vboid);

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
	
	glDrawArrays(GL_POLYGON, 0, size);
}

/*
TerrainGenerator::TerrainGenerator()
{ }

TerrainGenerator::TerrainGenerator(GLuint programID)
{ 
	program = programID;
	reset(); 
}

TerrainGenerator::~TerrainGenerator(void) { }

void TerrainGenerator::updateVao()
{
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			points[i][j] = 0;

			morePoints[i][j][0] = GRID_SIZE / 2 - i;
			morePoints[i][j][1] = points[i][j];
			morePoints[i][j][2] = GRID_SIZE / 2 - j;
		}
	}

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint aBuffer; 
	glGenBuffers(1, &aBuffer);
	myBuffer = aBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, myBuffer);
	//glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
	glBufferData(GL_ARRAY_BUFFER, sizeof(morePoints), morePoints, GL_STATIC_DRAW);


}

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