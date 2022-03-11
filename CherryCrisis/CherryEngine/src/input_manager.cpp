#include "pch.hpp"

#include <input_manager.hpp>

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
		NamedInput& current = m_context->m_namedKeys[inputName];

		return current.m_input;
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
		NamedInput& current = m_context->m_namedKeys[inputName];

		return current.m_input->m_isHeld;
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
		NamedInput& current = m_context->m_namedKeys[inputName];

		return current.m_input->m_isDown;
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
		NamedInput& current = m_context->m_namedKeys[inputName];

		return current.m_input->m_isUp;
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
		float totalValue = 0.f;
		NamedAxis& axis = m_context->m_axis[axisName];
		for (auto& input : axis.m_axis)
		{
			totalValue += input->m_value;
		}

		return totalValue;
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

void InputManager::MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_mouseWheel.x = xoffset; 
	m_mouseWheel.y = yoffset;
}

void InputManager::UpdateKeys() 
{
	m_mouseWheel = CCMaths::Vector2::Zero;

	if (m_context)
		m_context->Update();

	for (auto& key : m_framePressedKeys) 
	{
		Input& input = m_keys[key];

		input.m_isDown = false;
		input.m_isUp = false;
	}

	m_framePressedKeys.clear();
}


void InputManager::NamedInput::InvokeEvents()
{
	if (m_input->m_isDown)
		m_pressed.Invoke();

	else if (m_input->m_isHeld)
		m_held.Invoke();

	else if (m_input->m_isUp)
		m_released.Invoke();
}

void InputManager::NamedInput::AddInput(Keycode newInput)
{
	InputManager* inputManager = InputManager::instance();

	m_input = inputManager->GetInputRef(newInput);
}

void InputManager::NamedAxis::AddAxis(Axis* newAxis)
{
	m_axis.push_back(newAxis);
}

void InputManager::NamedAxis::InvokeEvents()
{
	float totalValue = 0.f;
	for (auto& input : m_axis)
	{
		totalValue += input->m_value;
	}
	
	if (m_oldValue != totalValue)
	{
		m_oldValue = totalValue;
		m_event.Invoke(m_oldValue);
	}
}

void InputManager::KeyboardContext::Update()
{
	for (auto& key : m_axis)
	{
		key.second.InvokeEvents();
	}

	for (auto& key : m_namedKeys)
	{
		key.second.InvokeEvents();
	}
}

void InputManager::KeyboardContext::AddAxisPreset(std::string name, Keycode neg, Keycode pos)
{
	if (!m_namedKeys.count(name))
		m_axis[name] = NamedAxis();
}

void InputManager::KeyboardContext::AddButtonPreset(std::string name, Keycode key)
{
	if (!m_namedKeys.count(name))
		m_namedKeys[name] = NamedInput();
}