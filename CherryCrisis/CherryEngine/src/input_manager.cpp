#include "pch.hpp"

#include "input_manager.hpp"

#include <debug.hpp>

template <>
InputManager* Singleton<InputManager>::currentInstance = nullptr;

const char* InputManager::GetKeyname(int index)
{
	if (index >= 0 && index < 122)
		return keynames[index];

	return keynames[0];
}

const char* InputManager::GetKeyname(Keycode code)
{
	for (int i = 0; i < 122; ++i)
	{
		if (keycodes[i] == code)
			return keynames[i];
	}

	return keynames[0];
}

int InputManager::GetKeynameIndex(const char* name)
{
	for (int i = 0; i < 122; ++i)
	{
		if (keynames[i] == name)
			return i;
	}

	return 0;
}

Keycode	InputManager::GetKeycode(int index)
{
	if (index >= 0 && index < 122)
		return keycodes[index];

	return keycodes[0];
}

Keycode	InputManager::GetKeycode(const char* name)
{
	for (int i = 0; i < 122; ++i)
	{
		if (keynames[i] == name)
			return keycodes[i];
	}
	
	return keycodes[0];
}

int InputManager::GetKeycodeIndex(Keycode code)
{
	for (int i = 0; i < 122; ++i)
	{
		if (keycodes[i] == code)
			return i;
	}

	return 0;
}


#pragma region Errors
void InputManager::ErrorButtons(const char* name)
{
	static Debug* debug = Debug::GetInstance();

	std::string errorLog = "Action Buttons ";
	errorLog += name;
	errorLog += " is not set up.";

	debug->Log(errorLog.c_str());
}

void InputManager::ErrorAxes(const char* name)
{
	static Debug* debug = Debug::GetInstance();

	std::string errorLog = "Action Axes ";
	errorLog += name;
	errorLog += " is not set up.";

	debug->Log(errorLog.c_str());
}
#pragma endregion

#pragma region Context
InputManager::KeyboardContext* InputManager::AddContext(const std::string& name)
{
	if (!m_contexts.contains(name))
		m_contexts[name] = KeyboardContext();

	return &m_contexts[name];
}

void InputManager::SetContext(const std::string& name)
{
	if (m_contexts.empty() || !m_contexts.contains(name))
		return;

	m_activeContext = &m_contexts[name];
}

void InputManager::SetContext(KeyboardContext* context)
{
	m_activeContext = context;
}
#pragma endregion

#pragma region Callbacks
void InputManager::SetListening()
{
	m_isListening = true;
}

void InputManager::ResetListenedKey()
{
	m_listenedKey = -1;
}

void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (m_isListening)
	{
		m_listenedKey = GetKeycodeIndex((Keycode)key);
		m_isListening = false;
		return;
	}

	if (action == 2)	return;

	// action = 1 if pressed and 0 if released

	Input& input = m_keys[(Keycode)key];

	input.Set(action, action, !action);

	input.m_isUpdated.Invoke(&input);

	m_framePressedKeys.push_back((Keycode)key);
}

void InputManager::MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_mouseWheel.x = (float)xoffset;
	m_mouseWheel.y = (float)yoffset;
}

void InputManager::MousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	m_mouseDelta.x = m_mousePos.x - (float)xpos;
	m_mouseDelta.y = m_mousePos.y - (float)ypos;

	m_mousePos.x = (float)xpos;
	m_mousePos.y = (float)ypos;
}

void InputManager::MouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (m_isListening)
	{
		m_listenedKey = GetKeycodeIndex((Keycode)(button + 1000));
		m_isListening = false;
		return;
	}

	if (action == 2)	return;

	Input& input = m_keys[(Keycode)(button+1000)];

	input.Set(action, action, !action);

	input.m_isUpdated.Invoke(&input);

	m_framePressedKeys.push_back((Keycode)(button + 1000));
}

void InputManager::UpdateKeys()
{
	m_mouseWheel = CCMaths::Vector2::Zero;
	m_mouseDelta = CCMaths::Vector2::Zero;

	m_listenedKey = -1;

	for (auto& key : m_framePressedKeys)
	{
		Input& input = m_keys[key];

		input.Set(false, input.Held(), false);
	}

	m_framePressedKeys.clear();
}
#pragma endregion

#pragma region Input
void InputManager::Input::Set(bool down, bool held, bool up)
{
	m_isDown = down;
	m_isHeld = held;
	m_isUp = up;
}

InputManager::Input* InputManager::GetInputRef(Keycode key)
{
	return &m_keys[key];
}

bool InputManager::GetKeyDown(Keycode key)
{
	return m_keys[key].Down();
}

bool InputManager::GetKey(Keycode key)
{
	return m_keys[key].Held();
}

bool InputManager::GetKeyUp(Keycode key)
{
	return m_keys[key].Up();
}

bool InputManager::GetKeyDown(const char* inputName)
{
	if (!m_activeContext)
		return false;

	if (m_activeContext->m_buttons.empty() || !m_activeContext->m_buttons.contains(inputName))
	{
		ErrorButtons(inputName);

		return 0.f;
	}
	else
	{
		ActionButtons& current = m_activeContext->m_buttons[inputName];
		return current.CheckDown();
	}
}

bool InputManager::GetKey(const char* inputName)
{
	if (!m_activeContext)
		return false;

	if (m_activeContext->m_buttons.empty() || !m_activeContext->m_buttons.contains(inputName))
	{
		ErrorButtons(inputName);

		return false;
	}
	else
	{
		ActionButtons& current = m_activeContext->m_buttons[inputName];
		return current.CheckHeld();
	}
}

bool InputManager::GetKeyUp(const char* inputName)
{
	if (!m_activeContext)
		return false;

	if (m_activeContext->m_buttons.empty() || !m_activeContext->m_buttons.contains(inputName))
	{
		ErrorButtons(inputName);

		return 0.f;
	}
	else
	{
		ActionButtons& current = m_activeContext->m_buttons[inputName];
		return current.CheckUp();
	}
}
#pragma endregion

#pragma region ActionButtons
InputManager::ActionButtons* InputManager::AddActionButtons(const std::string& name, int& success)
{
	if (!m_activeContext)
	{
		success = -1;
		return nullptr;
	}

	if (!m_activeContext->m_buttons.contains(name))
	{
		m_activeContext->m_buttons[name] = ActionButtons();
		success = 1;
	}
	else
		success = 0;

	return &m_activeContext->m_buttons[name];
}

int InputManager::RenameActionButtons(const std::string& oldName, const std::string& newName)
{
	if (!m_activeContext)
		return -1;

	if (m_activeContext->m_buttons.contains(newName))
		return 0;

	auto button = m_activeContext->m_buttons.extract(oldName);
	button.key() = newName;
	m_activeContext->m_buttons.insert(move(button));

	return 1;
}

int InputManager::AddInputToAction(const std::string& name, Keycode key)
{
	if (!m_activeContext || !m_activeContext->m_buttons.contains(name))
		return -1;

	m_activeContext->m_buttons[name].AddInput(key);
	return 1;
}

int InputManager::AddInputToAction(ActionButtons* preset, Keycode key)
{
	if (!m_activeContext)
		return -1;

	preset->AddInput(key);
	return 1;
}

int InputManager::ChangeInputInAction(ActionButtons* action, Keycode oldKey, Keycode newKey)
{
	return action->ChangeInput(oldKey, newKey);
}

void InputManager::ActionButtons::SetPriorKey(EPriorKey key)
{
	if ((int)key == -1)
		m_priorKey = nullptr;

	else
	{
		InputManager* inputManager = InputManager::GetInstance();

		m_priorKey = inputManager->GetInputRef((Keycode)key);
	}
}


void InputManager::ActionButtons::AddInput(Keycode newInput)
{
	InputManager* inputManager = InputManager::GetInstance();

	m_inputs[newInput] = inputManager->GetInputRef(newInput);

	m_inputs[newInput]->m_isUpdated.Bind(&ActionButtons::Update, this);
}

int InputManager::ActionButtons::ChangeInput(Keycode oldKey, Keycode newKey)
{
	if (m_inputs.empty() || !m_inputs.contains(oldKey))
		return -1;

	if (m_inputs.contains(newKey))
		return 0;

	m_inputs[oldKey]->m_isUpdated.Unbind(&ActionButtons::Update, this);
	
	m_inputs.erase(oldKey);
	AddInput(newKey);

	return 1;
}


void InputManager::ActionButtons::Update(Input* input)
{
	if (!GetPriorKey())
		return;

	if (input->Down())
		m_pressed.Invoke();

	else if (input->Held())
		m_held.Invoke();

	else if (input->Up())
		m_released.Invoke();
}

bool InputManager::ActionButtons::GetPriorKey()
{
	if (!m_priorKey || m_priorKey->Held())
		return true;

	return false;
}

bool InputManager::ActionButtons::CheckDown()
{
	if (!GetPriorKey())
		return false;

	for (auto& input : m_inputs)
	{
		if (input.second->Down())
			return true;
	}

	return false;
}

bool InputManager::ActionButtons::CheckHeld()
{
	if (!GetPriorKey())
		return false;

	for (auto& input : m_inputs)
	{
		if (input.second->Held())
			return true;
	}

	return false;
}

bool InputManager::ActionButtons::CheckUp()
{
	if (!GetPriorKey())
		return false;

	for (auto& input : m_inputs)
	{
		if (input.second->Up())
			return true;
	}

	return false;
}
#pragma endregion

#pragma region Axis
float InputManager::GetAxis(Keycode posKey, Keycode negKey)
{
	return (float)GetKey(posKey) - (float)GetKey(negKey);
}

float InputManager::GetAxis(const char* axisName)
{
	if (!m_activeContext)
		return 0.f;

	if (m_activeContext->m_axes.empty() || !m_activeContext->m_axes.contains(axisName))
	{
		ErrorAxes(axisName);

		return 0.f;
	}
	else
	{
		float totalValue = 0.f;
		ActionAxes& axis = m_activeContext->m_axes[axisName];

		return axis.ComputeValue();
	}
}

void InputManager::SetKey(Axis* axis, Keycode key, bool isNeg)
{
	if (isNeg)
		SetNegativeKey(axis, key);
	else
		SetPositiveKey(axis, key);
}


void InputManager::SetPositiveKey(Axis* axis, Keycode key)
{
	axis->SetPositiveKey(key);
}

void InputManager::SetNegativeKey(Axis* axis, Keycode key)
{
	axis->SetNegativeKey(key);
}

void InputManager::Axis::BindUpdate()
{
	InputManager* inputManager = InputManager::GetInstance();
	inputManager->m_keys[m_positiveKey].m_isUpdated.Bind(&Axis::Update, this);
	inputManager->m_keys[m_negativeKey].m_isUpdated.Bind(&Axis::Update, this);
}

void InputManager::Axis::SetPositiveKey(Keycode key)
{
	InputManager* inputManager = InputManager::GetInstance();

	// TODO: Unbind old key update
	inputManager->m_keys[m_positiveKey].m_isUpdated.Unbind(&Axis::Update, this);

	m_positiveKey = key;
	inputManager->m_keys[m_positiveKey].m_isUpdated.Bind(&Axis::Update, this);
}

void InputManager::Axis::SetNegativeKey(Keycode key)
{
	InputManager* inputManager = InputManager::GetInstance();

	// TODO: Unbind old key update
	inputManager->m_keys[m_negativeKey].m_isUpdated.Unbind(&Axis::Update, this);

	m_negativeKey = key;
	inputManager->m_keys[m_negativeKey].m_isUpdated.Bind(&Axis::Update, this);
}

void InputManager::Axis::Update(Input* input)
{
	Value();

	m_isUpdated.Invoke();
}

float InputManager::Axis::Value()
{
	InputManager* inputManager = InputManager::GetInstance();
	m_value = inputManager->GetAxis(this->m_positiveKey, this->m_negativeKey);

	return m_value;
}
#pragma endregion

#pragma region ActionAxes
InputManager::ActionAxes* InputManager::AddActionAxes(const std::string& name, int& success)
{
	if (!m_activeContext)
	{
		success = -1;
		return nullptr;
	}

	if (!m_activeContext->m_axes.contains(name))
	{
		m_activeContext->m_axes[name] = ActionAxes();
		success = 1;
	}
	else
		success = 0;

	return &m_activeContext->m_axes[name];
}

int InputManager::RenameActionAxes(const std::string& oldName, const std::string& newName)
{
	if (!m_activeContext || !m_activeContext->m_axes.contains(oldName))
		return -1;

	if (m_activeContext->m_axes.contains(newName))
	{
		m_activeContext->m_axes[newName] = ActionAxes();
		return 0;
	}

	auto button = m_activeContext->m_axes.extract(oldName);
	button.key() = newName;
	m_activeContext->m_axes.insert(move(button));

	return 1;
}

int InputManager::AddAxisToAction(const std::string& name, Axis axis)
{
	if (!m_activeContext || !m_activeContext->m_axes.contains(name))
		return -1;

	m_activeContext->m_axes[name].AddAxis(new Axis(axis));
	return 1;
}

int InputManager::AddAxisToAction(ActionAxes* preset, Axis axis)
{
	if (!m_activeContext)
		return -1;

	preset->AddAxis(new Axis(axis));
	return 1;
}

InputManager::ActionAxes::~ActionAxes()
{
	for (auto& axis : m_axes)
	{
		delete axis;
	}
}

void InputManager::ActionAxes::AddAxis(Axis* newAxis)
{
	newAxis->BindUpdate();

	newAxis->m_isUpdated.Bind(&ActionAxes::Update, this);

	m_axes.push_back(newAxis);
}

void InputManager::ActionAxes::Update()
{
	m_event.Invoke(ComputeValue());
}

float InputManager::ActionAxes::ComputeValue()
{
	m_value = 0.f;
	for (auto& input : m_axes)
	{
		m_value += input->Value();
		m_value = CCMaths::Clamp(m_value, -1.f, 1.f);
	}

	return m_value;
}
#pragma endregion