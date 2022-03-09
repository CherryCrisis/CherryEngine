%include "enums.swg"

%{
	#include "keycode.hpp"

	#include "singleton.hpp"
	#include "input_manager.hpp"
%}

%include keycode.hpp

%nodefaultctor InputManager;

class InputManager
{
public:
	static InputManager* GetInstance()
	{
		if (!instantiateFlag.test_and_set())
		{
			if (!currentInstance)
				currentInstance = new T();
		}

		return currentInstance;
	}

	bool GetKey(Keycode key);
	bool GetKeyDown(Keycode key);
	bool GetKeyUp(Keycode key);
	float GetAxis(const char* axisName);
};