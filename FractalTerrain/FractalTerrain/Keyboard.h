#ifndef KEYBOARD_H
#define KEYBOARD_H

/* 
Future Work:
	Make special keys like shift and ctrl work.
	Write my own data oriented thing instead of using map.
*/

enum KeyState
{
	DOWN,
	HELD,
	DOWNHELD,
	UP,
	ALL
};

void keyboardUpdate(void);
void addKeyHandler(void(*)(int, KeyState), int, KeyState);

#endif


