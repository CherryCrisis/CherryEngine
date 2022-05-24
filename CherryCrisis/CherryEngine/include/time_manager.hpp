#pragma once

#include <memory>

#include <cherry_macros.hpp>

#include "singleton.hpp"


namespace std::chrono
{
	class time_zone;
}

struct CCENGINE_API FullDate
{
	unsigned int hours;
	unsigned int minutes;
	unsigned int seconds;
	unsigned int milliseconds;

	unsigned int year;
	unsigned int month;
	unsigned int day;
};

class CCENGINE_API Chrono
{
private :
	unsigned int m_oldTime = 0;
	
	//max chrono time < 1h
	unsigned int GetCurrentTimeInMs();

public:
	Chrono() = default;

	void Start();
	unsigned int Stop();
};

class CCENGINE_API TimeManager : public Singleton<TimeManager>
{
private:
	float m_deltaTime = 0.f;
	float m_elapsedTime = 0.f;
	float m_fixedDeltaTime = 0.016f;
	float m_lastTime = 0.f;
	float m_timeScale = 1.f;
	int	  m_fixedLoopCount = 0;

	const std::chrono::time_zone* m_currentZone = nullptr;

public:
	TimeManager();

	//GETTER AND SETTERS
	static FullDate GetCurrentTime();
	static float	GetDeltaTime()	  { return currentInstance->m_deltaTime; }
	static float	GetTimeScale()	  { return currentInstance->m_timeScale; }
	static float	GetFixedDeltaTime() { return currentInstance->m_fixedDeltaTime; }
	static int		GetFixedLoopCount() { return currentInstance->m_fixedLoopCount; }
	static double	GetElapsedTime()	 { return currentInstance->m_elapsedTime; }

	static void  SetTimeScale(float newTime)      { currentInstance->m_timeScale = newTime; }
	static void  SetFixedDeltaTime(float newTime) { currentInstance->m_fixedDeltaTime = newTime; }
	
	void  Update(const float time);
};
