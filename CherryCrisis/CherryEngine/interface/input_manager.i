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
	static bool GetKey(Keycode key);
	static bool GetKeyDown(Keycode key);
	static bool GetKeyUp(Keycode key);
	static bool GetKey(const char* keyName);
	static bool GetKeyDown(const char* keyName);
	static bool GetKeyUp(const char* keyName);
	static float GetAxis(const char* axisName);
	static CCMaths::Vector2 GetMouseWheel();
	static CCMaths::Vector2 GetMousePos();
	static CCMaths::Vector2 GetMouseDelta();
};