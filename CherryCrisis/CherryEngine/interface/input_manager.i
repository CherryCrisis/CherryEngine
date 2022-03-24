%include "enums.swg"

%{
	#include "keycode.hpp"

	#include "singleton.hpp"
	#include "input_manager.hpp"
%}

%include keycode.hpp
%include std_string.i

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
	bool GetKey(const char* keyName);
	bool GetKeyDown(const char* keyName);
	bool GetKeyUp(const char* keyName);
	float GetAxis(const char* axisName);
	CCMaths::Vector2 GetMouseWheel();
	CCMaths::Vector2 GetMousePos();

	// TODO: Remove this
	void SetContext(const std::string& name);
};