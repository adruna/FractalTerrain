#ifndef GLOBAL_VALUES_H
#define GLOBAL_VALUES_H

static double const PI = 3.14159265358979323846;
static double const PI_2 = PI/2.0f;
static double const ONE_DEG = PI/180.0f;

void errorCheck(const char * = "ERROR! %s");

struct ShaderProgram
{
public:
	unsigned int programID;
	unsigned int vertexID;
	unsigned int fragID;

	ShaderProgram(const char *, const char *);
	~ShaderProgram();
};

/*
This was back when I didn't really know c++, so these are just silly for now.

class Globals
{
public:

	static float randf(float lower = 0, float upper = 1) { return rand()/(float)RAND_MAX * (upper - lower) + lower; }
};
*/
#endif