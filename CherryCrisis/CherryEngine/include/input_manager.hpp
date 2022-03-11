#pragma once

#include <cherry_macros.hpp>

#include <unordered_map>
#include <vector>

#include "singleton.hpp"

#include "maths.hpp"

#include "keycode.hpp"

class KeyboardContext;
class Event;

struct GLFWwindow {};

class CCENGINE_API InputManager : public Singleton<InputManager>
{
public:
	class Input
	{
	public:
		bool	m_isDown = false;
		bool	m_isUp = false;
		bool	m_isHeld = false;

		Event* m_callbackEvent = nullptr;
	};

private:
	//Nested Classes

	class Axis 
	{
	public:
		Keycode m_negativeInput = {};
		Keycode m_positiveInput = {};
	};

	class KeyboardContext
	{
	private:

	public:
		std::unordered_map<std::string, Keycode> m_namedKeys;
		std::unordered_map<Keycode, Event> m_inputPreset;

		//list of axis (can be added by the user via the editor (internally modifying game keyboard context))
		std::unordered_map<std::string, Axis> m_axis;		//<const char* = axisName, Axis = axis class>
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
