#include "Keyboard.h"

FunctionInfo::FunctionInfo(void(*func)(unsigned char, KeyState), KeyState state)
{
	callback = func;
	keyState = state;
}

KeyHandler::KeyHandler(void(*func)(unsigned char, KeyState), KeyState state)
{ info.push_back(FunctionInfo(func, state)); }

std::map<unsigned char, KeyHandler> Keyboard::keyMap;
std::list<unsigned char> Keyboard::keysDown;

void(*Keyboard::idleCallback)();

void Keyboard::update()
{
	std::list<unsigned char>::iterator it;
	for (it = keysDown.begin(); it != keysDown.end(); it++)
	{
		unsigned char key = *it;

		handleCallbacks(key, DOWN);
	}
}

void Keyboard::setIdleCallback(void(_cdecl*func)())
{
	idleCallback = func;
}


void Keyboard::keyHandler(GLFWwindow * window, unsigned int key)
{
	for (std::list<unsigned char>::iterator it = keysDown.begin(); it != keysDown.end(); it++)
	{
		if ((*it) == key)
		{
			keyUpHandler(key,0,0);
			return;
		}
	}
	keyDownHandler(key, 0, 0);
}

void Keyboard::keyDownHandler(unsigned char key, int x, int y)
{
	handleCallbacks(key, DOWN);

	keysDown.push_back(key);
	keysDown.unique();
}

void Keyboard::keyUpHandler(unsigned char key, int x, int y)
{

	handleCallbacks(key, UP);

	keysDown.remove(key);
}

void Keyboard::addKeyHandler(void(*callback)(unsigned char, KeyState), unsigned char key, KeyState keyState)
{
	std::map<unsigned char, KeyHandler>::iterator it = keyMap.find(key);

	if (it == keyMap.end())
	{ keyMap[key] = KeyHandler(callback, keyState); }
	else
	{ it->second.info.push_back(FunctionInfo(callback, keyState));}

}

void Keyboard::handleCallbacks(unsigned char key, KeyState currentState)
{
	std::map<unsigned char, KeyHandler>::iterator keyHandler = keyMap.find(key);
	if (keyHandler != keyMap.end()) // Find out if there is a key handler here.
	{
		std::list<FunctionInfo>::iterator funcInfo;
		for (funcInfo = keyHandler->second.info.begin(); funcInfo != keyHandler->second.info.end(); funcInfo++)
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