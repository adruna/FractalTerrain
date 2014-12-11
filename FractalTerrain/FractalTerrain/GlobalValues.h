#ifndef GLOBAL_VALUES_H
#define GLOBAL_VALUES_H

static double const PI = 3.14159265358979323846;
static double const PI_2 = PI/2.0f;
static double const ONE_DEG = PI/180.0f;

void errorCheck(const char * = "ERROR! %s");

/*
3 unsigned ints that define the program ID, vertex shader ID, and fragment shader ID.
Can use constructor to load glsl files and destructor to clean up.

Future Work:
	Could make this less object oriented and make the constructor/destructor just functions.
*/
struct ShaderProgram
{
public:
	unsigned int programID;
	unsigned int vertexID;
	unsigned int fragID;

	ShaderProgram(const char *, const char *);
	~ShaderProgram();
};

#endif