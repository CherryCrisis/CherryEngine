
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

        // Auto Scrolling ISSUE#38
        if (m_isAutoScrolling)
            ImGui::SetScrollHereY(1.0f);

        // Collapsing ISSUE#39
        for (const auto message : m_debug->GetLogs())
            ImGui::Text(message.c_str());
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
        if (ImGui::Button("Clear")) { Clear(); }
    }
    ImGui::EndMenuBar();
}

void LogDisplayer::Clear() 
{
    // Add Clearing code
}