#include "pch.hpp"

#include <input_manager.hpp>

#include <debug.hpp>

template <>
InputManager* Singleton<InputManager>::currentInstance = nullptr;


InputManager::Input* InputManager::GetInputRef(Keycode key)
{
	return &m_keys[key];
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
	return (float)GetKey(posKey) - (float)GetKey(negKey);
}

void InputManager::Error(const char* name)
{
	static Debug* debug = Debug::GetInstance();

	std::string errorLog = "Action ";
	errorLog += name;
	errorLog += " is not set up.";

	debug->Log(errorLog.c_str());
}

bool InputManager::GetKey(const char* inputName)
{
	if (m_activeContext->m_namedKeys.empty() || !m_activeContext->m_namedKeys.count(inputName))
	{
		Error(inputName);

		return false;
	}
	else
	{
		NamedInput& current = m_activeContext->m_namedKeys[inputName];
		for (auto& input : current.m_inputs)
		{
			if (input.second->m_isHeld)
				return true;
		}

		return false;
	}
}

bool InputManager::GetKeyDown(const char* inputName)
{
	if (m_activeContext->m_namedKeys.empty() || !m_activeContext->m_namedKeys.count(inputName))
	{
		Error(inputName);

		return 0.f;
	}
	else
	{
		NamedInput& current = m_activeContext->m_namedKeys[inputName];
		for (auto& input : current.m_inputs)
		{
			if (input.second->m_isDown)
				return true;
		}

		return false;
	}
}

bool InputManager::GetKeyUp(const char* inputName)
{
	if (m_activeContext->m_namedKeys.empty() || !m_activeContext->m_namedKeys.count(inputName))
	{
		Error(inputName);

		return 0.f;
	}
	else
	{
		NamedInput& current = m_activeContext->m_namedKeys[inputName];
		for (auto& input : current.m_inputs)
		{
			if (input.second->m_isUp)
				return true;
		}

		return false;
	}
}

float InputManager::GetAxis(const char* axisName)
{
	if (m_activeContext->m_axis.empty() || !m_activeContext->m_axis.count(axisName))
	{
		Error(axisName);

		return 0.f;
	}
	else
	{
		float totalValue = 0.f;
		NamedAxis& axis = m_activeContext->m_axis[axisName];
		for (auto& input : axis.m_axis)
		{
			totalValue += input->m_value;
			CCMaths::Clamp(totalValue, -1.f, 1.f);
		}

		return totalValue;
	}
}

void InputManager::SetContext(KeyboardContext* context) 
{
	m_activeContext = context;
}

void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	if (action == 2)	return;

	// action = 1 if pressed and 0 if released

	Input& input = m_keys[(Keycode)key];

	input.m_isHeld = action;
	input.m_isDown = action;
	input.m_isUp   = !action;

	//input.m_isUpdated.Invoke(&input);

	m_framePressedKeys.push_back((Keycode)key);
}

void InputManager::MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_mouseWheel.x = (float)xoffset; 
	m_mouseWheel.y = (float)yoffset;
}

void InputManager::UpdateKeys() 
{
	m_mouseWheel = CCMaths::Vector2::Zero;

	for (auto& key : m_framePressedKeys) 
	{
		Input& input = m_keys[key];

		input.m_isDown = false;
		input.m_isUp = false;
	}

	m_framePressedKeys.clear();
}

void InputManager::Axis::Update(Input* input)
{
	InputManager* inputManager = InputManager::instance();
	m_value = inputManager->GetAxis(this->m_positiveInput, this->m_negativeInput);

	m_isUpdated.Invoke(m_value);
}

void InputManager::NamedInput::Update(Input* input)
{
	if (input->m_isDown)
		m_pressed.Invoke();

	else if (input->m_isHeld)
		m_held.Invoke();

	else if (input->m_isUp)
		m_released.Invoke();
}

void InputManager::NamedInput::AddInput(Keycode newInput)
{
	InputManager* inputManager = InputManager::instance();
	
	m_inputs[newInput] = inputManager->GetInputRef(newInput);

	//m_inputs[newInput]->m_isUpdated.Bind(&NamedInput::Update, this);
}

void InputManager::NamedAxis::AddAxis(Axis* newAxis)
{
	newAxis->m_isUpdated.Bind(&NamedAxis::Update, this);

	m_axis.push_back(newAxis);
}

void InputManager::NamedAxis::Update(const float& value)
{
	for (auto& input : m_axis)
	{
		m_oldValue += input->m_value;
	}
	
	m_event.Invoke(m_oldValue);
}

void InputManager::KeyboardContext::AddAxisPreset(std::string name)
{
	if (!m_namedKeys.count(name))
		m_axis[name] = NamedAxis();
}

void InputManager::KeyboardContext::AddButtonPreset(std::string name)
{
	if (!m_namedKeys.count(name))
		m_namedKeys[name] = NamedInput();
}