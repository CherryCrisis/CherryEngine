
#include <input_manager.hpp>

//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>

#include <debug.hpp>
#include <string>



template <>
InputManager* Singleton<InputManager>::currentInstance = nullptr;

bool InputManager::GetKey(Keycode key)
{
	return m_keys[key].m_isPressed;
}

bool InputManager::GetKeyDown(Keycode key)
{
	return false;
}

bool InputManager::GetKeyUp(Keycode key)
{
	return false;
}

float InputManager::GetAxis(const char* axisName) 
{
	return 0.f;
}

void InputManager::SetContext(KeyboardContext* context) 
{
	return;
}

void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	// action = 1 if pressed and 0 if released 
	//Input input = m_keys[key];
	m_keys[(Keycode)key].m_isPressed = action;

	std::string str = std::to_string(key);

	if (action)
		Debug::GetInstance()->Log(str.c_str());
	//Add Event Broadcast
}