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
	static InputManager* GetInstance();

	bool GetKey(Keycode key);
	bool GetKeyDown(Keycode key);
	bool GetKeyUp(Keycode key);
	bool GetKey(const char* keyName);
	bool GetKeyDown(const char* keyName);
	bool GetKeyUp(const char* keyName);
	float GetAxis(const char* axisName);
	CCMaths::Vector2 GetMouseWheel();
	CCMaths::Vector2 GetMousePos();
	CCMaths::Vector2 GetMouseDelta();

	// TODO: Remove this
	void SetContext(const std::string& name);
};