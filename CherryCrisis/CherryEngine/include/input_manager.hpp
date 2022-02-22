#pragma once

#include "singleton.hpp"

#include <unordered_map>
#include <vector>

#include <CherryMacros.h>

#include "keycode.hpp"

class KeyboardContext;
class Event;
struct GLFWwindow
{

};

class CCENGINE_API InputManager : public Singleton<InputManager>
{
private:
	//Nested Classes
	class Input 
	{
	public:
		bool   m_isPressed     = false;
		bool   m_wasPressed    = false;
		Event* m_callbackEvent = nullptr;
	};

	class Axis 
	{
	public:
		float m_value = 0.f;
		const char* name = "";

		Keycode m_negativeInput = {};
		Keycode m_positiveInput = {};
	};


	//list of keys (intern glfw callback update key statut)
	std::unordered_map<Keycode, Input> m_keys;

	//list of axis (can be added by the user via the editor (internally modifying game keyboard context))
	std::unordered_map<const char*, Axis> m_axes; //<const char* = axisName, Axis = axis class>
	
	// Context (presets of differents callbacks and axes)
	KeyboardContext* m_context = nullptr;
public:

	bool GetKey(Keycode key);
	bool GetKeyDown(Keycode key);
	bool GetKeyUp(Keycode key);
	float GetAxis(const char* axisName);

	void SetContext(KeyboardContext* context);

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	/*
	void InputCallback(GLFWWindow int) 
	{
		m_keys[code].pressed = true;
		m_keys[code].Broadcast();
	}
	
	m_keys[Keycode.E].events += Foo;*/
};

// InputManager:GetKeyDown(Keycode.E);
// InputManager::GetAxis("axis");
// InputManager::GetKeyDown("a");
