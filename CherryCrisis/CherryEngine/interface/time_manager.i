%{
	#include "singleton.hpp"
	#include "time_manager.hpp"
%}

%nodefaultctor TimeManager;

%rename (Time) TimeManager;

class TimeManager
{
public:
	static TimeManager* GetInstance();

	float GetDeltaTime();
	float GetTimeScale();
	float GetFixedDeltaTime();
	double GetElapsedTime();

	void  SetTimeScale(float newTime);
	void  SetFixedDeltaTime(float newTime);
};