
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

        for (LogMessage& message : m_debug->GetLogs())
        {
            if (m_isCollapsing) 
            {
                ImGui::Text(message.CollapsedString().c_str());
            }
            else 
            {
                for (int i = 0; i < message.count; i++) 
                {
                    ImGui::Text(message.string.c_str());
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
    if(ImGui::BeginMenuBar())
    {
        if (ImGui::Checkbox("Clear on play", &m_isClearOnPlay));
        if (ImGui::Checkbox("AutoScroll", &m_isAutoScrolling));   
        if (ImGui::Checkbox("Collapse", &m_isCollapsing));

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
