%include "enums.swg"

%{
	#include "keycode.hpp"

	#include "singleton.hpp"
	#include "input_manager.hpp"
%}

%include keycode.hpp

%include singleton.i

%template(SingletonInput) Singleton<InputManager>;

class InputManager : public Singleton<InputManager>
{
public:
	bool GetKey(Keycode key);
	bool GetKeyDown(Keycode key);
	bool GetKeyUp(Keycode key);
	float GetAxis(const char* axisName);
};

