#pragma once

#include <unordered_map>

enum class Keycode;
class Event;
class Axis;

class KeyboardContext 
{
private:

public:
	std::unordered_map<Keycode, Event> m_inputPresets;
};