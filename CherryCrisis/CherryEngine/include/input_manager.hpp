#pragma once

#include "singleton.hpp"

#include <unordered_map>
#include <vector>

#include "event.hpp"
#include "cherry_header.hpp"
#include "keycode.hpp"

class KeyboardContext;

struct GLFWwindow {};

class CCENGINE_API InputManager : public Singleton<InputManager>
{
private:
	class Input
	{
	private:
		bool	m_isDown = false;
		bool	m_isHeld = false;
		bool	m_isUp = false;

	public:
		Event<Input*> m_isUpdated;

		void Set(bool down, bool held, bool up);

		bool Down() { return m_isDown; }	// Return true if input is pressed
		bool Held() { return m_isHeld; }	// Return true if input is held
		bool Up() { return m_isUp; }		// Return true if input is released
	};

	class Axis
	{
	private:
		Keycode m_positiveKey = {};
		Keycode m_negativeKey = {};

		float m_value = 0;

		void Update(Input* input);

	public:
		Event<> m_isUpdated;

		Axis(Keycode pos, Keycode neg)
			: m_positiveKey(pos), m_negativeKey(neg) {}
		
		void BindUpdate();
		float Value();	// Updates and returns value

		void SetPositiveKey(Keycode key);
		void SetNegativeKey(Keycode key);

		Keycode GetPositiveKey() { return m_positiveKey; }
		Keycode GetNegativeKey() { return m_negativeKey; }
	};

	class ActionButtons
	{
	private:
		std::unordered_map<Keycode, Input*> m_inputs = {};

		void Update(Input* input);

	public:
		Event<> m_pressed;					// Event called when the Action is pressed
		Event<> m_held;						// Event called when the Action is held
		Event<> m_released;					// Event called when the Action is release

		void AddInput(Keycode newInput);	// Add a new key to the action

		int ChangeInput(Keycode oldKey, Keycode newKey);

		bool CheckDown();					// Return true if Action (any input off m_inputs) is pressed
		bool CheckHeld();					// Return true if Action (any input off m_inputs) is held
		bool CheckUp();						// Return true if Action (any input off m_inputs) is released

		const std::unordered_map<Keycode, Input*>& Inputs() { return m_inputs; }
	};

	class ActionAxes
	{
	private:
		std::vector<Axis*> m_axes;

		float m_value = 0.f;

		void Update();

	public:
		Event<const float&> m_event;

		~ActionAxes();

		void AddAxis(Axis* newInput);	// Add Axis to Action

		float ComputeValue();			// Updates and returns total value

		const std::vector<Axis*>& Axes() { return m_axes; }

	};

public:
	struct KeyboardContext
	{
		std::unordered_map<std::string, ActionAxes> m_axes;
		std::unordered_map<std::string, ActionButtons> m_buttons;
	};

private:
	std::unordered_map<std::string, KeyboardContext> m_contexts;

	// Context (presets of differents callbacks and axes)
	KeyboardContext* m_activeContext = nullptr;

	//list of keys (intern glfw callback update key statut)
	std::unordered_map<Keycode, Input> m_keys;
	
	//list of mouse keys (intern glfw callback update key statut)
	std::unordered_map<Keycode, Input> m_mouseKeys;

	//list of keys just pressed
	std::vector<Keycode> m_framePressedKeys;
	
	CCMaths::Vector2 m_mouseWheel {};
	CCMaths::Vector2 m_mousePos   {};

	const char* keynames[122] =
	{
		"NONE",
		"SPACE",
		"APOSTROPHE",
		"COMMA",
		"MINUS",
		"PERIOD",
		"SLASH",

		"NUM_0",
		"NUM_1",
		"NUM_2",
		"NUM_3",
		"NUM_4",
		"NUM_5",
		"NUM_6",
		"NUM_7",
		"NUM_8",
		"NUM_9",

		"SEMICOLON",
		"EQUAL",

		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",

		"LEFT_BRACKET",
		"BACKSLASH",
		"RIGHT_BRACKET",
		"GRAVE_ACCENT",

		"ESCAPE",
		"ENTER",
		"TAB",
		"BACKSPACE",
		"INSERT",
		"DEL",
		"RIGHT",
		"LEFT",
		"DOWN",
		"UP",
		"PAGE_UP",
		"PAGE_DOWN",
		"HOME",
		"END",
		"CAPS_LOCK",
		"SCROLL_LOCK",
		"NUM_LOCK",
		"PRINT_SCREEN",
		"PAUSE",

		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",
		"F13",
		"F14",
		"F15",
		"F16",
		"F17",
		"F18",
		"F19",
		"F20",
		"F21",
		"F22",
		"F23",
		"F24",
		"F25",

		"KP_0",
		"KP_1",
		"KP_2",
		"KP_3",
		"KP_4",
		"KP_5",
		"KP_6",
		"KP_7",
		"KP_8",
		"KP_9",

		"KP_DECIMAL",
		"KP_DIVIDE",
		"KP_MULTIPLY",
		"KP_SUBTRACT",
		"KP_ADD",
		"KP_ENTER",
		"KP_EQUAL",

		"LEFT_SHIFT",
		"LEFT_CONTROL",
		"LEFT_ALT",
		"LEFT_SUPER",
		"RIGHT_SHIFT",
		"RIGHT_CONTROL",
		"RIGHT_ALT",
		"RIGHT_SUPER",

		"MENU",

		"LEFT_CLICK",
		"RIGHT_CLICK",
		"MIDDLE_CLICK"
	};

	std::vector<Keycode> keycodes =
	{
		Keycode::UNKNOWN,
		Keycode::SPACE,
		Keycode::APOSTROPHE,
		Keycode::COMMA,
		Keycode::MINUS,
		Keycode::PERIOD,
		Keycode::SLASH,

		Keycode::NUM_0,
		Keycode::NUM_1,
		Keycode::NUM_2,
		Keycode::NUM_3,
		Keycode::NUM_4,
		Keycode::NUM_5,
		Keycode::NUM_6,
		Keycode::NUM_7,
		Keycode::NUM_8,
		Keycode::NUM_9,

		Keycode::SEMICOLON,
		Keycode::EQUAL,

		Keycode::A,
		Keycode::B,
		Keycode::C,
		Keycode::D,
		Keycode::E,
		Keycode::F,
		Keycode::G,
		Keycode::H,
		Keycode::I,
		Keycode::J,
		Keycode::K,
		Keycode::L,
		Keycode::M,
		Keycode::N,
		Keycode::O,
		Keycode::P,
		Keycode::Q,
		Keycode::R,
		Keycode::S,
		Keycode::T,
		Keycode::U,
		Keycode::V,
		Keycode::W,
		Keycode::X,
		Keycode::Y,
		Keycode::Z,

		Keycode::LEFT_BRACKET,
		Keycode::BACKSLASH,
		Keycode::RIGHT_BRACKET,
		Keycode::GRAVE_ACCENT,

		Keycode::ESCAPE,
		Keycode::ENTER,
		Keycode::TAB,
		Keycode::BACKSPACE,
		Keycode::INSERT,
		Keycode::DEL,
		Keycode::RIGHT,
		Keycode::LEFT,
		Keycode::DOWN,
		Keycode::UP,
		Keycode::PAGE_UP,
		Keycode::PAGE_DOWN,
		Keycode::HOME,
		Keycode::END,
		Keycode::CAPS_LOCK,
		Keycode::SCROLL_LOCK,
		Keycode::NUM_LOCK,
		Keycode::PRINT_SCREEN,
		Keycode::PAUSE,

		Keycode::F1,
		Keycode::F2,
		Keycode::F3,
		Keycode::F4,
		Keycode::F5,
		Keycode::F6,
		Keycode::F7,
		Keycode::F8,
		Keycode::F9,
		Keycode::F10,
		Keycode::F11,
		Keycode::F12,
		Keycode::F13,
		Keycode::F14,
		Keycode::F15,
		Keycode::F16,
		Keycode::F17,
		Keycode::F18,
		Keycode::F19,
		Keycode::F20,
		Keycode::F21,
		Keycode::F22,
		Keycode::F23,
		Keycode::F24,
		Keycode::F25,

		Keycode::KP_0,
		Keycode::KP_1,
		Keycode::KP_2,
		Keycode::KP_3,
		Keycode::KP_4,
		Keycode::KP_5,
		Keycode::KP_6,
		Keycode::KP_7,
		Keycode::KP_8,
		Keycode::KP_9,

		Keycode::KP_DECIMAL,
		Keycode::KP_DIVIDE,
		Keycode::KP_MULTIPLY,
		Keycode::KP_SUBTRACT,
		Keycode::KP_ADD,
		Keycode::KP_ENTER,
		Keycode::KP_EQUAL,

		Keycode::LEFT_SHIFT,
		Keycode::LEFT_CONTROL,
		Keycode::LEFT_ALT,
		Keycode::LEFT_SUPER,
		Keycode::RIGHT_SHIFT,
		Keycode::RIGHT_CONTROL,
		Keycode::RIGHT_ALT,
		Keycode::RIGHT_SUPER,

		Keycode::MENU,

		Keycode::LEFT_CLICK,
		Keycode::RIGHT_CLICK,
		Keycode::MIDDLE_CLICK
	};

public:
	
	//Errors
	void ErrorButtons(const char* Name);
	void ErrorAxes(const char* Name);

	// Context
	KeyboardContext* AddContext(const std::string& name);

	void SetContext(const std::string& name);
	void SetContext(KeyboardContext* context);

	// Callbacks
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
	void MousePosCallback(GLFWwindow* window, double xpos, double ypos);
	void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);
	
	void UpdateKeys();

	// Input
	Input* GetInputRef(Keycode key);

	bool GetKeyDown(Keycode key);
	bool GetKeyDown(const char* inputName);

	bool GetKey(Keycode key);
	bool GetKey(const char* inputName);

	bool GetKeyUp(Keycode key);
	bool GetKeyUp(const char* inputName);

	// ActionButtons
	ActionButtons* AddActionButtons(const std::string& name, int& success);
	int RenameActionButtons(const std::string& oldName, const std::string& newName);

	int AddInputToAction(const std::string& name, Keycode key);
	int AddInputToAction(ActionButtons* preset, Keycode key);

	int ChangeInputInAction(ActionButtons* action, Keycode oldKey, Keycode newKey);
	
	// Axis
	void SetPositiveKey(Axis* axis, Keycode key);
	void SetNegativeKey(Axis* axis, Keycode key);

	float GetAxis(Keycode posKey, Keycode negKey);
	float GetAxis(const char* axisName);

	// ActionAxes
	ActionAxes* AddActionAxes(const std::string& name, int& success);
	int RenameActionAxes(const std::string& oldName, const std::string& newName);

	int AddAxisToAction(const std::string& name, Axis axis);
	int AddAxisToAction(ActionAxes* preset, Axis axis);

	// Get
	CCMaths::Vector2 GetMouseWheel() { return m_mouseWheel; }
	CCMaths::Vector2 GetMousePos()   { return m_mousePos;   }

	int			KeynamesSize() { return 122; }

	const char* GetKeyname(int index);
	const char* GetKeyname(Keycode code);
	int			GetKeynameIndex(const char* name);

	Keycode		GetKeycode(int index);
	Keycode		GetKeycode(const char* name);
	int			GetKeycodeIndex(Keycode code);
};
