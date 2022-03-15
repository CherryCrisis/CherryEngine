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

		Event<Input*> m_isUpdated;
	};

	struct Axis
	{
		Keycode m_negativeInput = {};
		Keycode m_positiveInput = {};

		float m_value = 0;

		Event<const float&> m_isUpdated;

		void Update(Input* input);

		Axis(Keycode neg, Keycode pos)
			: m_negativeInput(neg), m_positiveInput(pos) {}
	};

	struct NamedInput
	{
		std::unordered_map<Keycode, Input*> m_inputs = {};

		Event<> m_pressed;
		Event<> m_held;
		Event<> m_released;

		void Update(Input* input);

		void AddInput(Keycode newInput);
	};

	struct NamedAxis
	{
		std::vector<Axis*> m_axis;
		Event<const float&> m_event;

		float m_oldValue = 0.f;

		void Update(const float& value);

		void AddAxis(Axis* newInput);
	};

	struct KeyboardContext
	{
		std::unordered_map<std::string, NamedAxis> m_axis;
		std::unordered_map<std::string, NamedInput> m_namedKeys;
	};

private:
	std::unordered_map<std::string, KeyboardContext> m_contexts;

	//list of keys (intern glfw callback update key statut)
	std::unordered_map<Keycode, Input> m_keys;
	
	//list of keys just pressed
	std::vector<Keycode> m_framePressedKeys;

	// Context (presets of differents callbacks and axes)
	KeyboardContext* m_activeContext = nullptr;
	
	CCMaths::Vector2 m_mouseWheel {};

public:

	void Error(const char* Name);

	Input* GetInputRef(Keycode key);

	bool GetKey(Keycode key);
	bool GetKeyDown(Keycode key);
	bool GetKeyUp(Keycode key);

	float GetAxis(Keycode posKey, Keycode negKey);

	bool GetKey(const char* inputName);
	bool GetKeyDown(const char* inputName);
	bool GetKeyUp(const char* inputName);

	float GetAxis(const char* axisName);

	CCMaths::Vector2 GetMouseWheel() { return m_mouseWheel; }

	KeyboardContext* AddContext(std::string name);
	void SetContext(KeyboardContext* context);

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
	void UpdateKeys();

	void AddKeyToPreset(std::string name, Keycode key);
	void AddKeyToPreset(NamedInput* preset, Keycode key);
	void AddAxisToPreset(std::string name, Axis* axis);
	void AddAxisToPreset(NamedAxis* preset, Axis* axis);

	NamedAxis* AddAxisPreset(std::string name);
	NamedInput* AddButtonPreset(std::string name);
};
