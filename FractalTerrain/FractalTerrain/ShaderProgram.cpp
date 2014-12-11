#include <iostream>
#include <GL/glew.h>
#include "GlobalValues.h"

/*
Standard glfw error check.
Optional message since glfw messages are usualy pretty pathetic.
Use %s in your message to place the glfw error in the console notification.
*/
void errorCheck(const char *message)
{
	GLenum ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			message, 
			gluErrorString(ErrorCheckValue)
			);
		fprintf(stderr, "\n");
	}
}

// Obtained from Jessica Bayliss (who obtained it from an example of Angel code for graphics).

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile)
{
	//FILE* fp = fopen(shaderFile, "r"); // VS gives warning about being insecure :-)

	// one way to deal with warning: http://stackoverflow.com/questions/2575116/fopen-fopen-s-and-writing-to-files:
	FILE* fp;
	#ifdef WIN32
		errno_t err;
		if ((err = fopen_s(&fp, shaderFile, "r")) != 0) 
		{
			#else
			if ((fopen_s(&fp, shaderFile, "r")) != 0) {
	#endif
	fprintf(stderr, "Cannot open config file %s!\n", shaderFile);
	}


	if (fp == NULL) { return NULL; }
	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);

	buf[size] = '\0';
	fclose(fp);

	return buf;
}

/*
Constructor for a shader program.
Takes the vertex shader source file and fragment shader source file.
*/
ShaderProgram::ShaderProgram(const char *vertexSourceFile, const char *fragSourceFile)
{
	const char *vertexShaderSource = readShaderSource(vertexSourceFile);
	const char *fragShaderSource = readShaderSource(fragSourceFile);

	programID = glCreateProgram();

	vertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexID, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexID);
	// TODO: Check that its compiled.

	fragID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragID, 1, &fragShaderSource, NULL);
	glCompileShader(fragID);
	// TODO: Check that its compiled.

	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragID);

	glLinkProgram(programID);
	// Check linked properly.
}

/*
Cleans up shaders.
*/
ShaderProgram::~ShaderProgram()
{
	glDetachShader(programID, vertexID);
	glDetachShader(programID, fragID);

	// Order matters!!
	glDeleteShader(fragID);
	glDeleteShader(vertexID);

	glDeleteProgram(programID);
}


// Old code for error checking later.
/*
GLint  linked;
glGetProgramiv(programID, GL_LINK_STATUS, &linked);
if (!linked) {
	std::cerr << "Shader program failed to link" << std::endl;
	GLint  logSize;
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logSize);
	char* logMsg = new char[logSize];
	glGetProgramInfoLog(programID, logSize, NULL, logMsg);
	std::cerr << logMsg << std::endl;
	delete[] logMsg;
}
// Create a GLSL program object from vertex and fragment shader files
GLuint initShader(const char* vShaderFile, const char* fShaderFile)
{
    for ( int i = 0; i < 2; ++i ) {

	GLuint shader = glCreateShader( s.type );
	glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
	glCompileShader( shader );

	GLint  compiled;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
	if ( !compiled ) {
	    std::cerr << s.filename << " failed to compile:" << std::endl;
	    GLint  logSize;
	    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
	    char* logMsg = new char[logSize];
	    glGetShaderInfoLog( shader, logSize, NULL, logMsg );
	    std::cerr << logMsg << std::endl;
	    delete [] logMsg;

	    exit( EXIT_FAILURE );
	}

	delete [] s.source;

	glAttachShader( program, shader );
    }

    // link  and error check 
    glLinkProgram(program);

    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
	std::cerr << "Shader program failed to link" << std::endl;
	GLint  logSize;
	glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
	char* logMsg = new char[logSize];
	glGetProgramInfoLog( program, logSize, NULL, logMsg );
	std::cerr << logMsg << std::endl;
	delete [] logMsg;

	exit( EXIT_FAILURE );
    }

    // use program object 
    glUseProgram(program);

    return program;
}
*/