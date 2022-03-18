#pragma once

#include "singleton.hpp"
#include "cherry_header.hpp"


class CCENGINE_API TimeManager : public Singleton<TimeManager>
{
private:
	float m_deltaTime = 0.f;
	float m_timeScale = 1.f;
	float m_fixedDeltaTime = 1.f/60.f;
	float m_elapsedTime = 0.f;

	double m_lastTime = 0.f;

public:
	
	//GETTER AND SETTERS
	float GetDeltaTime()	  { return m_deltaTime; }
	float GetTimeScale()	  { return m_timeScale; }
	float GetFixedDeltaTime() { return m_fixedDeltaTime; }
	double GetElapsedTime()	  { return m_elapsedTime; }

	void  SetTimeScale(float newTime)      { m_timeScale = newTime; }
	void  SetFixedDeltaTime(float newTime) { m_fixedDeltaTime = newTime; }
    //

	void  Update(const double time);
};
