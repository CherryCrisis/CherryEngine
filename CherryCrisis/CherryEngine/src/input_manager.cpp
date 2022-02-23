
#include <input_manager.hpp>

//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>

#include <debug.hpp>
#include <string>
#include <iostream>


template <>
InputManager* Singleton<InputManager>::currentInstance = nullptr;


bool InputManager::GetKey(Keycode key)
{
	return m_keys[key].m_isDown;
}

bool InputManager::GetKeyDown(Keycode key)
{
	return m_keys[key].m_isDown && !m_keys[key].m_wasPressed;
}

bool InputManager::GetKeyUp(Keycode key)
{
	return !m_keys[key].m_isDown && m_keys[key].m_wasPressed;
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
	if (key == 2)	return;
	// action = 1 if pressed and 0 if released 
	//Input input = m_keys[key];
	Input& input = m_keys[(Keycode)key];

	input.m_isDown = action;
}

void InputManager::UpdateKeys() 
{
	for (auto& key : m_keys) 
	{
		Input& input = key.second;

		input.m_wasPressed = input.m_isDown;
	}
}