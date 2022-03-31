
#include "panels/log_displayer.hpp"

#include <imgui.h>

#include "debug.hpp"

LogDisplayer::LogDisplayer()
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

            for (auto it = collapsedLogs->begin(); it != collapsedLogs->end(); ++it)
            {
                ImGui::Text(std::string("[" + LogTypeStr[(int)it->second.m_logType] + "] " + it->second.m_logMessage + " : " + std::to_string(it->second.m_count)).c_str());
            }
        }
        else
        {
            auto logs = m_debug->GetLogs();
            for (Log& log : *logs)
            {
                ImGui::Text(log.m_logMessage->m_logMessage.c_str());
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
    if(ImGui::BeginMenuBar())
    {
        ImGui::Checkbox("Clear on play", &m_isClearOnPlay);
        ImGui::Checkbox("AutoScroll", &m_isAutoScrolling);
        ImGui::Checkbox("Collapse", &m_isCollapsing);

        ImGui::Checkbox("Info", &m_displayInfo);
        ImGui::Checkbox("Warning", &m_displayWarning);
        ImGui::Checkbox("Error", &m_displayError);

        if (ImGui::Button("Clear"))         { Clear(); }
        if (ImGui::Button("Scroll Top"))    { m_isScrollingTop = true; }
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
