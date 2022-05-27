%{
	#include "singleton.hpp"
	#include "time_manager.hpp"
%}

%nodefaultctor TimeManager;

%rename (Time) TimeManager;

class TimeManager
{
public:
	static float GetDeltaTime();
	static float GetTimeScale();
	static float GetFixedDeltaTime();
	static double GetElapsedTime();

	static void  SetTimeScale(float newTime);
	static void  SetFixedDeltaTime(float newTime);
};