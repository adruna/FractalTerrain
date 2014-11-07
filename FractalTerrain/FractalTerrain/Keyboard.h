#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <map>
#include <list>
#include <GL\glfw3.h>

enum KeyState
{
	DOWN,
	HELD,
	DOWNHELD,
	UP,
	ALL
};

void keyboardGLFWHandler(GLFWwindow *, int, int, int, int);
void keyboardUpdate(void);
void addKeyHandler(void(*)(int, KeyState), int, KeyState);

/*
Manages keyboard input.
I was kinda proud of this back in the day, but I should probably use whatever glfw provides for input.

class Keyboard
{
private:
	static std::map<unsigned char, KeyHandler> keyMap;
	static std::list<unsigned char> keysDown;

	static void(*idleCallback)();
	static void handleCallbacks(unsigned char, KeyState);

public:

	static void update(void);
	static void setIdleCallback(void(*)());
	
	static void keyHandler(GLFWwindow *, int, int, int, int);
	static void keyDownHandler(unsigned char, int, int);
	static void keyUpHandler(unsigned char, int, int);

	static void addKeyHandler(void(*)(unsigned char, KeyState), unsigned char, KeyState);
};
*/
#endif


