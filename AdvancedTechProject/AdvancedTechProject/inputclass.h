#pragma once
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_


class InputClass	//will store the state of each key in a keyboard array. When quieried it will tell the calling functions if a certain key is pressed
					//given input from the SystemClass:MessageHandler Function
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();
	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	bool IsKeyDown(unsigned int);


private:
	bool m_keys[256]; //array of keyboard keys
};

#endif
