#include "Keyboard.h"
#include "KeyboardHandler.h"

#include <map>
#include <list>

/*
Structure that holds a function to call and the key state in which to call it for.
*/
struct FunctionInfo
{
	KeyState keyState;
	void(*callback)(int, KeyState);

	FunctionInfo(void(*func)(int, KeyState) = nullptr, KeyState state = DOWN)
	{ callback = func; keyState = state; };
};

std::map<int, std::list<FunctionInfo>> keyMap;
std::list<int> keysDown;

/*
Determines whether or not to call the callback given the current key state.
*/
void handleCallbacks(unsigned char key, KeyState currentState)
{
	std::map<int, std::list<FunctionInfo>>::iterator keyHandler = keyMap.find(key);
	if (keyHandler != keyMap.end()) // Find out if there is a key handler here.
	{
		std::list<FunctionInfo>::iterator funcInfo;
		for (funcInfo = keyHandler->second.begin(); funcInfo != keyHandler->second.end(); funcInfo++)
		{ // Loop through and call any that need to be called.
			bool call = false;
			switch (funcInfo->keyState)
			{
			case DOWN:
				if (currentState == funcInfo->keyState)
				{ call = true; }
				break;
			case HELD:
				if (currentState == funcInfo->keyState)
				{ call = true; }
				break;
			case DOWNHELD:
				if (currentState == HELD || currentState == DOWN)
				{ call = true; }
				break;
			case UP:
				if (currentState == funcInfo->keyState)
				{ call = true; }
				break;
			case ALL:
				call = true;
				break;
			default:
				break;
			}//THANK YOU
			
			if (call)
				funcInfo->callback(key, currentState);
		}
	}
}

/*
Handles the Down key state based on the list of keys down.
Needs to be called once per frame (TODO, remove + re-implement).
*/
void keyboardUpdate(void)
{
	std::list<int>::iterator it = keysDown.begin();
	std::list<int>::iterator end = keysDown.end();
	for (; it != end; it++)
	{
		int key = *it;

		handleCallbacks(key, DOWN);
	}
}

/*
Repeat handler, calls handle callbacks.
GLFWKeyFunction dosn't fire repeats during the windows key delay.
void keyRepeatHandler(int key)
{
	handleCallbacks(key, HELD);
}
*/

/*
Press handler, calls handle callbacks and puts into keysDown.
*/
void keyPressHandler(int key)
{
	handleCallbacks(key, DOWN);

	keysDown.push_back(key);
	keysDown.unique();
}

/*
Release handler, calls handle callbacks and removes from keys down.
*/
void keyReleaseHandler(int key)
{
	handleCallbacks(key, UP);

	keysDown.remove(key);
}

/*
GLFW key handler function.
*/
void keyboardGLFWHandler(GLFWwindow * window, int key, int scan, int action, int mode)
{
	switch (action)
	{
	case GLFW_RELEASE:
		keyReleaseHandler(key);
		break;
	case GLFW_PRESS:
		keyPressHandler(key);
		break;
	}
}

/*
Adds a keyhandler with specified callback, key, and keystate.
*/
void addKeyHandler(void(*callback)(int, KeyState), int key, KeyState keyState)
{
	std::map<int, std::list<FunctionInfo>>::iterator it = keyMap.find(key);

	if (it == keyMap.end())
	{ 
		keyMap[key] = std::list<FunctionInfo>();
		keyMap[key].push_back(FunctionInfo(callback, keyState));
	}
	else
	{ it->second.push_back(FunctionInfo(callback, keyState)); }
}

