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

struct FunctionInfo
{
	KeyState keyState;
	void(*callback)(unsigned char, KeyState);

	FunctionInfo(void(*)(unsigned char, KeyState) = nullptr, KeyState = DOWN);
};

struct KeyHandler
{
	std::list<FunctionInfo> info;
	KeyHandler(void(*)(unsigned char, KeyState) = nullptr, KeyState = DOWN);
};

/*
Manages keyboard input.
I was kinda proud of this back in the day, but I should probably use whatever glfw provides for input.
*/
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
	
	static void keyHandler(GLFWwindow *, unsigned int);
	static void keyDownHandler(unsigned char, int, int);
	static void keyUpHandler(unsigned char, int, int);

	static void addKeyHandler(void(*)(unsigned char, KeyState), unsigned char, KeyState);
};

#endif


