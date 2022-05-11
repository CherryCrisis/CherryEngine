#pragma once

#include "singleton.hpp"

#include <unordered_map>
#include <stack>
#include <vector>

#include "event.hpp"
#include "cherry_header.hpp"
#include "keycode.hpp"

class InputContext;

struct GLFWwindow {};

//! enum of Keycodes that can be use as prior keys for Action of type button
enum class EPriorKey
{
	NONE = -1,
	LEFT_SHIFT = 340,
	LEFT_CONTROL = 341,
	LEFT_ALT = 342,
	RIGHT_SHIFT = 344,
	RIGHT_CONTROL = 345,
	RIGHT_ALT = 346,
};


class CCENGINE_API InputManager : public Singleton<InputManager>
{
public:
	class CCENGINE_API Input
	{
	private:
		bool	m_isDown = false;
		bool	m_isHeld = false;
		bool	m_isUp = false;

	public:
		Event<Input*> m_isUpdated;

		//! \hiderefs
		void Set(bool down, bool held, bool up);

		//! Return true if input is pressed
		/*!
		*	Return true if input is pressed
		*	\note Prefere using InputManager::GetKeyDown(Keycode)
		*/
		bool Down() { return m_isDown; }

		//! Return true if input is held
		/*!
		*	Return true if input is held
		*	\note Prefere using InputManager::GetKey(Keycode)
		*/
		bool Held() { return m_isHeld; }

		//! Return true if input is released
		/*!
		*	Return true if input is released
		*	\note Prefere using InputManager::GetKeyUp(Keycode)
		*/
		bool Up() { return m_isUp; }
	};

	class CCENGINE_API Axis
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
		
		//! \hiderefs
		void BindUpdate();

		//! \hiderefs
		void Destroy();

		//! \hiderefs
		float Value();

		void SetPositiveKey(Keycode key);
		void SetNegativeKey(Keycode key);

		Keycode GetPositiveKey() { return m_positiveKey; }
		Keycode GetNegativeKey() { return m_negativeKey; }
	};

	class CCENGINE_API ActionSingle
	{
	private:
		Input* m_priorKey = nullptr;
		std::unordered_map<Keycode, Input*> m_inputs = {};

		void Update(Input* input);

	public:
		Event<> m_pressed;					// Event called when the Action is pressed
		Event<> m_held;						// Event called when the Action is held
		Event<> m_released;					// Event called when the Action is release

		void SetPriorKey(EPriorKey key);

		void AddInput(Keycode newInput);	// Add a new key to the action
		void RemoveInput(Keycode oldKey);	// Add a new key to the action
		int	 ChangeInput(Keycode oldKey, Keycode newKey);
		void Destroy();

		std::string GetAsString(Keycode key);

		bool GetPriorKey();

		//! Return true if any \b key of the Action is pressed
		/*!
		*	Return true if any \b key of the Action is pressed
		*	\note Prefere using InputManager::GetKeyDown(const char*)
		*/
		bool CheckDown();

		//! Return true if any \b key of the Action is held
		/*!
		*	Return true if any \b key of the Action is held
		*	\note Prefere using InputManager::GetKey(const char*)
		*/
		bool CheckHeld();
		
		//! Return true if any \b key of the Action is released
		/*!
		*	Return true if any \b key of the Action is released
		*	\note Prefere using InputManager::GetKeyUp(const char*)
		*/
		bool CheckUp();

		const std::unordered_map<Keycode, Input*>& Inputs() { return m_inputs; }
	};

	class CCENGINE_API ActionAxes
	{
	private:
		std::vector<Axis*> m_axes;

		float m_value = 0.f;

		void Update();

	public:		
		Event<const float&> m_event;

		//! \hiderefs
		~ActionAxes();

		//! Add new axis to action
		/*!
		*	Add new axis \b newInput in Action
		*	\note Avoid using this method. Use InputManager::AddAxisToAction() instead
		*/
		void AddAxis(Axis* newAxis);
		
		void RemoveAxis(Axis* newAxis);

		//! \hiderefs
		void Destroy();

		//! \hiderefs
		float ComputeValue();

		std::string GetAsString(Axis* axis);

		const std::vector<Axis*>& Axes() { return m_axes; }
	};

	struct CCENGINE_API InputContext
	{
		//list of keys (intern glfw callback update key statut)
		std::unordered_map<Keycode, Input> m_keys;
		std::vector<Keycode> m_framePressedKeys;

		std::unordered_map<std::string, ActionAxes> m_axes;
		std::unordered_map<std::string, ActionSingle> m_single;

		CCMaths::Vector2 m_mouseWheel{};
		CCMaths::Vector2 m_mouseDelta{};
	};

private:
	std::unordered_map<std::string, InputContext> m_contexts;

	// Context (presets of differents callbacks and axes)
	InputContext* m_pollContext = nullptr;
	InputContext* m_defaultContext = nullptr;	
	std::stack<InputContext*> m_fetchContext;

	//list of keys just pressed
	bool m_isListening = false;
	int m_listenedKey = -1;
	CCMaths::Vector2 m_mousePos{};

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
	
	InputManager();
	~InputManager();

	//Errors
	void ErrorButtons(const char* Name);
	void ErrorAxes(const char* Name);

	// Context
	InputContext* GetOrAddContext(const std::string& name);
	InputContext* GetContext(const std::string& name);

	void SetPollContext(const std::string& name);
	void SetPollContext(InputContext* context);

	void PushContext(const std::string& name);
	void PushContext(InputContext* context);

	void PopContext();

	void SetDefaultContext();

	void (*HideCursor)(void* window);
	void (*ShowCursor)(void* window);

	// Callbacks
	void SetListening();
	void ResetListenedKey();

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
	void MousePosCallback(GLFWwindow* window, double xpos, double ypos);
	void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);
	
	void UpdateKeys();

	// Input
	Input* GetInputRef(Keycode key);

	//! Return true if the \b key is pressed
	/*!
	*	Return true if the \b key is pressed
	*/
	bool GetKeyDown(Keycode key);

	//! Return true if any \b key of the Action is pressed
	/*!
	*	Return true if any \b key of the Action named \b inputName is pressed
	*/
	bool GetKeyDown(const char* inputName);


	//! Return true if the \b key is held
	/*!
	*	Return true if the \b key is held
	*/
	bool GetKey(Keycode key);

	//! Return true if any \b key of the Action is held
	/*!
	*	Return true if any \b key of the Action named \b inputName is held
	*/
	bool GetKey(const char* inputName);


	//! Return true if the \b key is released
	/*!
	*	Return true if the \b key is released
	*/
	bool GetKeyUp(Keycode key);
	
	//! Return true if any \b key of the Action is released
	/*!
	*	Return true if any \b key of the Action named \b inputName is released
	*/
	bool GetKeyUp(const char* inputName);

	void SetCursorHidden();
	void SetCursorDisplayed();

	// ActionButtons
	//! Add a new Action of type single input
	/*!
	*	Add a new Action of type button in the currently active \b fetchContext
	*	\note Avoid using this method. Add the Action through the Project Settings instead
	*/
	ActionSingle*	AddActionSingle(const std::string& name, int& success);
	int				RemoveActionSingle(const std::string& name);

	int RenameActionButtons(const std::string& oldName, const std::string& newName);

	//! Set the prior key of the Action
	/*!
	*	Set the prior key of the Action \b name as \b priorKey 
	*/
	void SetActionPriorKey(const std::string& name, EPriorKey priorKey);


	//! Add new input to action
	/*!
	*	Add the \b key in the Action \b name
	*/
	int AddInputToAction(const std::string& name, Keycode key);
	
	//! Add new input to action
	/*!
	*	Add the \b key in the Action \b preset
	*/
	int AddInputToAction(ActionSingle* preset, Keycode key);

	//! Remove new input from action
	/*!
	*	Remove the \b key in the Action \b name
	*/
	int RemoveInputFromAction(const std::string& name, Keycode key);

	//! Remove new input from action
	/*!
	*	Remove the \b key in the Action \b preset
	*/
	int RemoveInputFromAction(ActionSingle* preset, Keycode key);

	//! \hiderefs
	int ChangeInputInAction(ActionSingle* action, Keycode oldKey, Keycode newKey);
	
	// Axis
	void SetKey(Axis* axis, Keycode key, bool isNeg);
	void SetPositiveKey(Axis* axis, Keycode key);
	void SetNegativeKey(Axis* axis, Keycode key);

	float GetAxis(Keycode posKey, Keycode negKey);
	float GetAxis(const char* axisName);

	// ActionAxes
	//! Add a new Action of type axe (double input)
	/*!
	*	Add a new Action of type axis in the currently active \b fetchContext
	*	\note Avoid using this method. Add the Action through the Project Settings instead
	*/
	ActionAxes* AddActionAxes(const std::string& name, int& success);
	int			RemoveActionAxes(const std::string& name);

	//! \hiderefs
	int RenameActionAxes(const std::string& oldName, const std::string& newName);

	//! \hiderefs
	Axis GetFromString(const std::string& in);

	//! Add new axis to action
	/*!
	*	Create a pointer on the the axis \b newInput and add it in the Action \b name
	*/
	int AddAxisToAction(const std::string& name, Axis axis);

	//! Add new axis to action
	/*!
	*	Create a pointer on the the axis \b newInput and add it in the Action \b preset
	*/
	int AddAxisToAction(ActionAxes* preset, Axis axis);

	//! Remove new axis to action
	/*!
	*	Create a pointer on the the axis \b newInput and add it in the Action \b name
	*/
	int RemoveAxisFromAction(const std::string& name, Axis* axis);

	//! Remove new axis to action
	/*!
	*	Create a pointer on the the axis \b newInput and add it in the Action \b preset
	*/
	int RemoveAxisFromAction(ActionAxes* preset, Axis* axis);

	// Get
	CCMaths::Vector2 GetMouseWheel();
	CCMaths::Vector2 GetMousePos();
	CCMaths::Vector2 GetMouseDelta();

	const int GetListenedKey() { return m_listenedKey; }

	int			KeynamesSize() { return 122; }

	const char* GetKeyname(int index);
	const char* GetKeyname(Keycode code);
	int			GetKeynameIndex(const char* name);

	Keycode		GetKeycode(int index);
	Keycode		GetKeycode(const char* name);
	int			GetKeycodeIndex(Keycode code);
};
