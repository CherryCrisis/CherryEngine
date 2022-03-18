#include <pch.hpp>

#include <time_manager.hpp>

template <>
TimeManager* Singleton<TimeManager>::currentInstance = nullptr;

void TimeManager::Update(const double time) 
{
	m_lastTime = m_elapsedTime;
	m_elapsedTime = time;
	m_deltaTime = (m_elapsedTime - m_lastTime) * m_timeScale;
}