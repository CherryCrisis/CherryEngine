#include "pch.hpp"

#include <input_manager.hpp>

//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>

#include <debug.hpp>

template <>
InputManager* Singleton<InputManager>::currentInstance = nullptr;


InputManager::Input* InputManager::GetInputRef(Keycode key)
{
	return &m_keys[key];
}

InputManager::Input* InputManager::GetInputRef(const char* inputName)
{
	if (m_context->m_namedKeys.empty() || !m_context->m_namedKeys.count(inputName))
	{
		Debug* debug = Debug::GetInstance();

		std::string errorLog = "Key ";
		errorLog += inputName;
		errorLog += " is not set up.";

		debug->Log(errorLog.c_str());

		return nullptr;
	}
	else
	{
		Keycode& current = m_context->m_namedKeys[inputName];

		return GetInputRef(current);
	}
}

bool InputManager::GetKey(Keycode key)
{
	return m_keys[key].m_isHeld;
}

bool InputManager::GetKeyDown(Keycode key)
{
	return m_keys[key].m_isDown;
}

bool InputManager::GetKeyUp(Keycode key)
{
	return m_keys[key].m_isUp;
}

float InputManager::GetAxis(Keycode posKey, Keycode negKey)
{
	return GetKey(posKey) - GetKey(negKey);
}

bool InputManager::GetKey(const char* inputName)
{
	if (m_context->m_namedKeys.empty() || !m_context->m_namedKeys.count(inputName))
	{
		Debug* debug = Debug::GetInstance();

		std::string errorLog = "Key ";
		errorLog += inputName;
		errorLog += " is not set up.";

		debug->Log(errorLog.c_str());

		return 0.f;
	}
	else
	{
		Keycode& current = m_context->m_namedKeys[inputName];

		return GetKey(current);
	}
}

bool InputManager::GetKeyDown(const char* inputName)
{
	if (m_context->m_namedKeys.empty() || !m_context->m_namedKeys.count(inputName))
	{
		Debug* debug = Debug::GetInstance();

		std::string errorLog = "Key ";
		errorLog += inputName;
		errorLog += " is not set up.";

		debug->Log(errorLog.c_str());

		return 0.f;
	}
	else
	{
		Keycode& current = m_context->m_namedKeys[inputName];

		return GetKeyDown(current);
	}
}

bool InputManager::GetKeyUp(const char* inputName)
{
	if (m_context->m_namedKeys.empty() || !m_context->m_namedKeys.count(inputName))
	{
		Debug* debug = Debug::GetInstance();

		std::string errorLog = "Key ";
		errorLog += inputName;
		errorLog += " is not set up.";

		debug->Log(errorLog.c_str());

		return 0.f;
	}
	else
	{
		Keycode& current = m_context->m_namedKeys[inputName];

		return GetKeyUp(current);
	}
}

float InputManager::GetAxis(const char* axisName) 
{
	if (m_context->m_axis.empty() || !m_context->m_axis.count(axisName))
	{
		Debug* debug = Debug::GetInstance();

		std::string errorLog = "Axis ";
		errorLog += axisName;
		errorLog += " is not set up.";

		debug->Log(errorLog.c_str());

		return 0.f;
	}
	else
	{
		Axis& current = m_context->m_axis[axisName];

		return GetKey(current.m_positiveInput) - GetKey(current.m_negativeInput);
	}
}

void InputManager::SetContext(KeyboardContext* context) 
{
	m_context = context;
}

void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	if (action == 2)	return;
	// action = 1 if pressed and 0 if released 
	//Input input = m_keys[key];
	Input& input = m_keys[(Keycode)key];

	input.m_isHeld = action;
	input.m_isDown = action;
	input.m_isUp   = !action;

	m_framePressedKeys.push_back((Keycode)key);
}

void InputManager::UpdateKeys() 
{
	for (auto& key : m_framePressedKeys) 
	{
		Input& input = m_keys[key];

		input.m_isDown = false;
		input.m_isUp = false;
	}

	m_framePressedKeys.clear();
}