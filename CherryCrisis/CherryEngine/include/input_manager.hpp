#pragma once

#include "singleton.hpp"

class KeyboardContext;
class Event;

class InputManger : public Singleton<InputManger>
{
private:
	//Nested 
	class Input 
	{
	public:
		bool   m_isPressed     = false;
		bool   m_wasPressed    = false;
		Event* m_callbackEvent = nullptr;
	};

	//list of keys
	

	// Context (presets of callbacks)
	KeyboardContext* m_context = nullptr;
public:

	bool GetKey();
	bool GetKeyDown();
	bool GetKeyUp();
};