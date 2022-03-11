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
public:
	struct Input
	{
		bool	m_isDown = false;
		bool	m_isUp = false;
		bool	m_isHeld = false;
	};

	struct Axis
	{
		Keycode m_negativeInput = {};
		Keycode m_positiveInput = {};

		float m_value = 0;

		Axis(Keycode neg, Keycode pos)
			: m_negativeInput(neg), m_positiveInput(pos) {}
	};

	struct NamedInput
	{
		Input* m_input;

		Event<> m_pressed;
		Event<> m_held;
		Event<> m_released;

		void AddInput(Keycode newInput);
		void InvokeEvents();
	};

	struct NamedAxis
	{
		std::vector<Axis*> m_axis;
		Event<float> m_event;

		int m_oldValue = 0.f;

		void AddAxis(Axis* newInput);
		void InvokeEvents();
	};

private:

	class KeyboardContext
	{
	public:
		std::unordered_map<std::string, NamedAxis> m_axis;
		std::unordered_map<std::string, NamedInput> m_namedKeys;

		void AddAxisPreset(std::string name, Keycode neg, Keycode pos);
		void AddButtonPreset(std::string name, Keycode key);

		void Update();
	};

	//list of keys (intern glfw callback update key statut)
	std::unordered_map<Keycode, Input> m_keys;
	
	//list of keys just pressed
	std::vector<Keycode> m_framePressedKeys;

	// Context (presets of differents callbacks and axes)
	KeyboardContext* m_context = nullptr;
	
	CCMaths::Vector2 m_mouseWheel {};

public:

	Input* GetInputRef(Keycode key);
	Input* GetInputRef(const char* inputName);

	bool GetKey(Keycode key);
	bool GetKeyDown(Keycode key);
	bool GetKeyUp(Keycode key);

	float GetAxis(Keycode posKey, Keycode negKey);

	bool GetKey(const char* inputName);
	bool GetKeyDown(const char* inputName);
	bool GetKeyUp(const char* inputName);

	float GetAxis(const char* axisName);

	CCMaths::Vector2 GetMouseWheel() { return m_mouseWheel; }

	void SetContext(KeyboardContext* context);

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
	void UpdateKeys();
};
