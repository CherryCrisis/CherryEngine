#include "pch.hpp"

#include "input_manager.hpp"

#include <debug.hpp>

template <>
InputManager* Singleton<InputManager>::currentInstance = nullptr;


InputManager::InputManager()
{
	m_defaultContext = GetOrAddContext("Editor Context");
	SetPollContext(m_defaultContext);
	PushContext(m_defaultContext);
}

InputManager::~InputManager()
{
	for (auto& context : m_contexts)
	{
		PushContext(&context.second);

		for (auto& action : context.second.m_single)
			action.second.Destroy();

		for (auto& action : context.second.m_axes)
			action.second.Destroy();

		context.second.m_single.clear();
		context.second.m_axes.clear();

		PopContext();
	}

	while (!m_fetchContext.empty())
		m_fetchContext.pop();

	m_contexts.clear();
}

CCMaths::Vector2 InputManager::GetMouseWheel()
{
	if (m_fetchContext.top())
		return m_fetchContext.top()->m_mouseWheel;

	return { 0 };
}

CCMaths::Vector2 InputManager::GetMousePos()
{
	return m_mousePos;
}

CCMaths::Vector2 InputManager::GetMouseDelta()
{
	if (m_fetchContext.top())
		return m_fetchContext.top()->m_mouseDelta;

	return { 0 };
}

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
	std::string currentName;
	std::string strName = name;

	for (int i = 0; i < 122; ++i)
	{
		currentName = keynames[i];
		if (currentName == strName)
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

	debug->AddLog(ELogType::ERROR, errorLog.c_str());
}

void InputManager::ErrorAxes(const char* name)
{
	static Debug* debug = Debug::GetInstance();

	std::string errorLog = "Action Axes ";
	errorLog += name;
	errorLog += " is not set up.";

	debug->AddLog(ELogType::ERROR, errorLog.c_str());
}
#pragma endregion

#pragma region Context
InputManager::InputContext* InputManager::GetOrAddContext(const std::string& name)
{
	return &m_contexts[name];
}

InputManager::InputContext* InputManager::GetContext(const std::string& name)
{
	if (!m_contexts.contains(name))
		return nullptr;

	return &m_contexts[name];
}

void InputManager::SetPollContext(const std::string& name)
{
	if (m_contexts.empty() || !m_contexts.contains(name))
		return;

	SetPollContext(&m_contexts[name]);
}

void InputManager::SetPollContext(InputContext* context)
{
	if (!context)
		context = m_defaultContext;

	if (m_pollContext)
	{
		for (auto& key : m_pollContext->m_keys)
			key.second.Set(false, false, false);

		m_pollContext->m_mouseDelta = { 0.f, 0.f };
		m_pollContext->m_mouseWheel = { 0.f, 0.f };
	}

	m_pollContext = context;
}

void InputManager::PushContext(const std::string& name)
{
	if (m_contexts.empty() || !m_contexts.contains(name))
		return;

	PushContext(&m_contexts[name]);
}

void InputManager::PushContext(InputContext* context)
{
	if (!context)
		context = m_defaultContext;

	m_fetchContext.push(context);
}

void InputManager::PopContext()
{
	if (!m_fetchContext.empty())
		m_fetchContext.pop();
}

void InputManager::SetDefaultContext()
{
	SetPollContext(m_defaultContext);
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

	Input& input = m_pollContext->m_keys[(Keycode)key];

	input.Set(action, action, !action);

	input.m_isUpdated.Invoke(&input);

	m_pollContext->m_framePressedKeys.push_back((Keycode)key);
}

void InputManager::MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	m_pollContext->m_mouseWheel.x = (float)xoffset;
	m_pollContext->m_mouseWheel.y = (float)yoffset;
}

void InputManager::MousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	m_pollContext->m_mouseDelta.x = m_mousePos.x - (float)xpos;
	m_pollContext->m_mouseDelta.y = m_mousePos.y - (float)ypos;

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

	Input& input = m_pollContext->m_keys[(Keycode)(button + 1000)];

	input.Set(action, action, !action);

	input.m_isUpdated.Invoke(&input);

	m_pollContext->m_framePressedKeys.push_back((Keycode)(button + 1000));
}

void InputManager::UpdateKeys()
{
	if (m_pollContext)
	{
		m_pollContext->m_mouseWheel = CCMaths::Vector2::Zero;
		m_pollContext->m_mouseDelta = CCMaths::Vector2::Zero;

		for (auto& key : m_pollContext->m_framePressedKeys)
		{
			Input& input = m_pollContext->m_keys[key];

			input.Set(false, input.Held(), false);
		}

		m_pollContext->m_framePressedKeys.clear();
	}
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
	return &m_fetchContext.top()->m_keys[key];
}


void InputManager::SetCursorHidden()
{
	HideCursor(Engine::window_handle);
}

void InputManager::SetCursorDisplayed()
{
	ShowCursor(Engine::window_handle);
}

bool InputManager::GetKeyDown(Keycode key)
{
	return m_fetchContext.top()->m_keys[key].Down();
}

bool InputManager::GetKey(Keycode key)
{
	return m_fetchContext.top()->m_keys[key].Held();
}

bool InputManager::GetKeyUp(Keycode key)
{
	return m_fetchContext.top()->m_keys[key].Up();
}

bool InputManager::GetKeyDown(const char* inputName)
{
	if (!m_fetchContext.top())
		return false;

	if (m_fetchContext.top()->m_single.empty() || !m_fetchContext.top()->m_single.contains(inputName))
	{
		ErrorButtons(inputName);
		return 0.f;
	}
	else
	{
		ActionSingle& current = m_fetchContext.top()->m_single[inputName];
		return current.CheckDown();
	}
}

bool InputManager::GetKey(const char* inputName)
{
	if (!m_fetchContext.top())
		return false;

	if (m_fetchContext.top()->m_single.empty() || !m_fetchContext.top()->m_single.contains(inputName))
	{
		ErrorButtons(inputName);

		return false;
	}
	else
	{
		ActionSingle& current = m_fetchContext.top()->m_single[inputName];
		return current.CheckHeld();
	}
}

bool InputManager::GetKeyUp(const char* inputName)
{
	if (!m_fetchContext.top())
		return false;

	if (m_fetchContext.top()->m_single.empty() || !m_fetchContext.top()->m_single.contains(inputName))
	{
		ErrorButtons(inputName);

		return 0.f;
	}
	else
	{
		ActionSingle& current = m_fetchContext.top()->m_single[inputName];
		return current.CheckUp();
	}
}
#pragma endregion

#pragma region ActionSingle
InputManager::ActionSingle* InputManager::AddActionSingle(const std::string& name, int& success)
{
	if (!m_fetchContext.top())
	{
		success = -1;
		return nullptr;
	}

	if (!m_fetchContext.top()->m_single.contains(name))
	{
		success = 1;
	}
	else
		success = 0;

	return &m_fetchContext.top()->m_single[name];
}

int InputManager::RemoveActionSingle(const std::string& name)
{
	if (!m_fetchContext.top())
		return -1;

	if (m_fetchContext.top()->m_single.contains(name))
	{
		m_fetchContext.top()->m_single.erase(name);
		return 1;
	}
	else
		return -1;
}

int InputManager::RenameActionButtons(const std::string& oldName, const std::string& newName)
{
	if (!m_fetchContext.top())
		return -1;

	if (m_fetchContext.top()->m_single.contains(newName))
		return 0;

	auto button = m_fetchContext.top()->m_single.extract(oldName);
	button.key() = newName;
	m_fetchContext.top()->m_single.insert(move(button));

	return 1;
}

void InputManager::SetActionPriorKey(const std::string& name, EPriorKey priorKey)
{
	if (!m_fetchContext.top() || !m_fetchContext.top()->m_single.contains(name))
		return;

	m_fetchContext.top()->m_single[name].SetPriorKey(priorKey);
}

int InputManager::AddInputToAction(const std::string& name, Keycode key)
{
	if (!m_fetchContext.top() || !m_fetchContext.top()->m_single.contains(name))
		return -1;

	m_fetchContext.top()->m_single[name].AddInput(key);
	return 1;
}

int InputManager::AddInputToAction(ActionSingle* preset, Keycode key)
{
	if (!m_fetchContext.top())
		return -1;

	preset->AddInput(key);
	return 1;
}

int InputManager::RemoveInputFromAction(const std::string& name, Keycode key)
{
	if (!m_fetchContext.top() || !m_fetchContext.top()->m_single.contains(name))
		return -1;

	m_fetchContext.top()->m_single[name].RemoveInput(key);
	return 1;
}

int InputManager::RemoveInputFromAction(ActionSingle* preset, Keycode key)
{
	if (!m_fetchContext.top())
		return -1;

	preset->RemoveInput(key);
	return 1;
}

int InputManager::ChangeInputInAction(ActionSingle* action, Keycode oldKey, Keycode newKey)
{
	return action->ChangeInput(oldKey, newKey);
}

void InputManager::ActionSingle::SetPriorKey(EPriorKey key)
{
	if ((int)key == -1)
		m_priorKey = nullptr;

	else
	{
		InputManager* inputManager = InputManager::GetInstance();

		m_priorKey = inputManager->GetInputRef((Keycode)key);
	}
}


void InputManager::ActionSingle::AddInput(Keycode newInput)
{
	Input* input = InputManager::GetInstance()->GetInputRef(newInput);

	m_inputs[newInput] = input;

	input->m_isUpdated.Bind(&ActionSingle::Update, this);
}

void InputManager::ActionSingle::RemoveInput(Keycode oldKey)
{
	Input* input = InputManager::GetInstance()->GetInputRef(oldKey);

	input->m_isUpdated.Unbind(&ActionSingle::Update, this);

	m_inputs.erase(oldKey);
}

int InputManager::ActionSingle::ChangeInput(Keycode oldKey, Keycode newKey)
{
	if (m_inputs.empty() || !m_inputs.contains(oldKey))
		return -1;

	if (m_inputs.contains(newKey))
		return 0;

	RemoveInput(oldKey);
	AddInput(newKey);

	return 1;
}

void InputManager::ActionSingle::Destroy()
{
	InputManager* IM = InputManager::GetInstance();

	for (auto& input : m_inputs)
	{
		IM->GetInputRef(input.first)->m_isUpdated.Unbind(&ActionSingle::Update, this);
	}
	m_inputs.clear();
}

std::string InputManager::ActionSingle::GetAsString(Keycode key)
{
	InputManager* IM = InputManager::GetInstance();

	if (m_inputs.contains(key))
		return IM->GetKeyname(key);

	return IM->GetKeyname(Keycode::UNKNOWN);
}


void InputManager::ActionSingle::Update(Input* input)
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

bool InputManager::ActionSingle::GetPriorKey()
{
	if (!m_priorKey || m_priorKey->Held())
		return true;

	return false;
}

bool InputManager::ActionSingle::CheckDown()
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

bool InputManager::ActionSingle::CheckHeld()
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

bool InputManager::ActionSingle::CheckUp()
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
	if (!m_fetchContext.top())
		return 0.f;

	if (m_fetchContext.top()->m_axes.empty() || !m_fetchContext.top()->m_axes.contains(axisName))
	{
		ErrorAxes(axisName);

		return 0.f;
	}
	else
	{
		float totalValue = 0.f;
		ActionAxes& axis = m_fetchContext.top()->m_axes[axisName];

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
	InputContext* context = InputManager::GetInstance()->m_fetchContext.top();
	context->m_keys[m_positiveKey].m_isUpdated.Bind(&Axis::Update, this);
	context->m_keys[m_negativeKey].m_isUpdated.Bind(&Axis::Update, this);
}

void InputManager::Axis::Destroy()
{
	InputContext* context = InputManager::GetInstance()->m_fetchContext.top();
	context->m_keys[m_positiveKey].m_isUpdated.Unbind(&Axis::Update, this);
	context->m_keys[m_negativeKey].m_isUpdated.Unbind(&Axis::Update, this);
}

void InputManager::Axis::SetPositiveKey(Keycode key)
{
	InputContext* context = InputManager::GetInstance()->m_fetchContext.top();

	context->m_keys[m_positiveKey].m_isUpdated.Unbind(&Axis::Update, this);
	
	m_positiveKey = key;
	context->m_keys[m_positiveKey].m_isUpdated.Bind(&Axis::Update, this);
}

void InputManager::Axis::SetNegativeKey(Keycode key)
{
	InputContext* context = InputManager::GetInstance()->m_fetchContext.top();

	// Unbind update of previous key
	context->m_keys[m_negativeKey].m_isUpdated.Unbind(&Axis::Update, this);

	// Bind update of new key
	m_negativeKey = key;
	context->m_keys[m_negativeKey].m_isUpdated.Bind(&Axis::Update, this);
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
	if (!m_fetchContext.top())
	{
		success = -1;
		return nullptr;
	}

	if (!m_fetchContext.top()->m_axes.contains(name))
	{
		success = 1;
	}
	else
		success = 0;

	return &m_fetchContext.top()->m_axes[name];
}

int InputManager::RemoveActionAxes(const std::string& name)
{
	if (!m_fetchContext.top())
		return -1;

	if (m_fetchContext.top()->m_axes.contains(name))
	{
		m_fetchContext.top()->m_axes[name].Destroy();
		m_fetchContext.top()->m_axes.erase(name);
		return 1;
	}
	else
		return -1;
}

int InputManager::RenameActionAxes(const std::string& oldName, const std::string& newName)
{
	if (!m_fetchContext.top() || !m_fetchContext.top()->m_axes.contains(oldName))
		return -1;

	if (m_fetchContext.top()->m_axes.contains(newName))
	{
		m_fetchContext.top()->m_axes[newName] = ActionAxes();
		return 0;
	}

	auto button = m_fetchContext.top()->m_axes.extract(oldName);
	button.key() = newName;
	m_fetchContext.top()->m_axes.insert(move(button));

	return 1;
}

int InputManager::AddAxisToAction(const std::string& name, Axis axis)
{
	if (!m_fetchContext.top() || !m_fetchContext.top()->m_axes.contains(name))
		return -1;

	m_fetchContext.top()->m_axes[name].AddAxis(new Axis(axis));
	return 1;
}

int InputManager::AddAxisToAction(ActionAxes* preset, Axis axis)
{
	if (!m_fetchContext.top())
		return -1;

	preset->AddAxis(new Axis(axis));
	return 1;
}

int InputManager::RemoveAxisFromAction(const std::string& name, Axis* axis)
{
	if (!m_fetchContext.top() || !m_fetchContext.top()->m_axes.contains(name))
		return -1;

	m_fetchContext.top()->m_axes[name].RemoveAxis(axis);
	return 1;
}

int InputManager::RemoveAxisFromAction(ActionAxes* preset, Axis* axis)
{
	if (!m_fetchContext.top())
		return -1;

	preset->RemoveAxis(axis);
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

void InputManager::ActionAxes::RemoveAxis(Axis* newAxis)
{
	newAxis->Destroy();

	newAxis->m_isUpdated.Unbind(&ActionAxes::Update, this);

	for (auto& axis : m_axes)
	{
		if (newAxis == axis)
		{
			axis = m_axes.back();
			m_axes.pop_back();
			delete newAxis;
		}
	}
}

void InputManager::ActionAxes::Destroy()
{
	for (auto& axis : m_axes)
	{
		axis->m_isUpdated.Unbind(&ActionAxes::Update, this);
		delete axis;
		axis = nullptr;
	}

	m_axes.clear();
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

std::string InputManager::ActionAxes::GetAsString(Axis* axis)
{
	InputManager* IM = InputManager::GetInstance();
	
	for (auto& currAxis : m_axes)
	{
		if (axis == currAxis)
		{
			std::string out = IM->GetKeyname(axis->GetPositiveKey());
			out += "/";
			out += IM->GetKeyname(axis->GetNegativeKey());
			return out;
		}
	}

	std::string out = IM->GetKeyname(Keycode::UNKNOWN);
	out += "/";
	out += IM->GetKeyname(Keycode::UNKNOWN);
	return out;
}

InputManager::Axis InputManager::GetFromString(const std::string& in)
{
	InputManager* IM = InputManager::GetInstance();

	Keycode	pos = IM->GetKeycode(in.substr(0, in.find("/")).c_str());
	Keycode neg = IM->GetKeycode(in.substr(in.find("/") + 1, in.size() - 1).c_str());

	return {pos, neg};
}