#include <pch.hpp>

#include "time_manager.hpp"

#include <chrono>

template <>
TimeManager* Singleton<TimeManager>::currentInstance = nullptr;

TimeManager::TimeManager()
{
    m_currentZone = std::chrono::current_zone();
}

void TimeManager::Update(const float time) 
{
	m_lastTime = m_elapsedTime;
	m_elapsedTime = time;
	m_deltaTime = (m_elapsedTime - m_lastTime) * m_timeScale;
}

FullDate TimeManager::GetCurrentTime()
{
    auto tp = std::chrono::zoned_time{ m_currentZone, std::chrono::system_clock::now() }.get_local_time();
    auto dp = floor<std::chrono::days>(tp);

    std::chrono::year_month_day ymd{ dp };

    std::chrono::hh_mm_ss time{ floor<std::chrono::milliseconds>(tp - dp) };

    return FullDate{
        .hours = static_cast<unsigned int>(time.hours().count()),
        .minutes = static_cast<unsigned int>(time.minutes().count()),
        .seconds = static_cast<unsigned int>(time.seconds().count()),
        .milliseconds = static_cast<unsigned int>(time.subseconds().count()),
        .year = static_cast<unsigned int>(static_cast<int>(ymd.year())),
        .month = static_cast<unsigned int>(ymd.month()),
        .day = static_cast<unsigned int>(ymd.day()),
    };
}

unsigned int Chrono::GetCurrentTimeInMs()
{
    FullDate date = TimeManager::GetInstance()->GetCurrentTime();
    return date.milliseconds + date.seconds * 1000 + date.minutes * 60000;
}

void Chrono::Start()
{
    m_oldTime = GetCurrentTimeInMs();
}

unsigned int Chrono::Stop()
{
    return GetCurrentTimeInMs() - m_oldTime;
}