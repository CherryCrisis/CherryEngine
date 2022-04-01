
#include "panels/log_displayer.hpp"

#include <imgui.h>

#include "debug.hpp"

LogDisplayer::LogDisplayer()
    : m_showedLogMask(1 << (int)ELogType::INFO | 1 << (int)ELogType::WARNING | 1 << (int)ELogType::ERROR)
{
    m_debug = Debug::GetInstance();
}

void LogDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Logs", &m_isOpened, ImGuiWindowFlags_MenuBar))
    {
        RenderMenuBar();

        if (m_isCollapsing)
        {
            auto collapsedLogs = m_debug->GetCollapsedLogs();

            for (auto it = collapsedLogs->rbegin(); it != collapsedLogs->rend(); ++it)
            {
                if (m_showedLogMask & 1 << (int)it->second.m_logType)
                {
                    ImGui::Text("[%s] %s : %i",
                        LogTypeCstr[(int)it->second.m_logType],
                        it->second.m_logMessage.c_str(),
                        it->second.m_count);
                }
            }
        }
        else
        {
            auto logs = m_debug->GetLogs();
            for (auto it = logs->rbegin(); it != logs->rend(); ++it)
            {
                if (m_showedLogMask & 1 << (int)it->m_logMessage->m_logType)
                {
                    ImGui::Text("%02u:%02u:%02u [%s] %s",
                        it->m_date.hours,
                        it->m_date.minutes,
                        it->m_date.seconds,
                        LogTypeCstr[(int)it->m_logMessage->m_logType],
                        it->m_logMessage->m_logMessage.c_str());
                }
            }
        }

        if (m_isAutoScrolling && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);


        if (m_isScrollingBot)
        {
            ImGui::SetScrollHereY(1.0f);
            m_isScrollingBot = false;
        }

        if (m_isScrollingTop)
        {
            ImGui::SetScrollHereY(0.f);
            m_isScrollingTop = false;
        }
    }
    ImGui::End();
}

void LogDisplayer::RenderMenuBar() 
{
    if (ImGui::BeginMenuBar())
    {
        ImGui::Checkbox("Clear on play", &m_isClearOnPlay);
        ImGui::Checkbox("AutoScroll", &m_isAutoScrolling);
        ImGui::Checkbox("Collapse", &m_isCollapsing);

        if (ImGui::Checkbox("Info", &m_displayInfo))
        {
            if (m_displayInfo)
                m_showedLogMask |= 1 << (int)ELogType::INFO;
            else
                m_showedLogMask &= ~(1 << (int)ELogType::INFO);
        }

        if (ImGui::Checkbox("Warning", &m_displayWarning))
        {
            if (m_displayWarning)
                m_showedLogMask |= 1 << (int)ELogType::WARNING;
            else
                m_showedLogMask &= ~(1 << (int)ELogType::WARNING);
        }

        if (ImGui::Checkbox("Error", &m_displayError))
        {
            if (m_displayError)
                m_showedLogMask |= 1 << (int)ELogType::ERROR;
            else
                m_showedLogMask &= ~(1 << (int)ELogType::ERROR);
        }

        if (ImGui::Button("Clear")) { Clear(); }
        if (ImGui::Button("Scroll Top")) { m_isScrollingTop = true; }
        if (ImGui::Button("Scroll Bottom")) { m_isScrollingBot = true; }
    }

    ImGui::EndMenuBar();
}

void LogDisplayer::Clear() 
{
    // Add Clearing code
    m_debug->Clear();
}

void LogDisplayer::TryClearOnPlay()
{
    // Add Clearing code
    if (m_isClearOnPlay)
        m_debug->Clear();
}
