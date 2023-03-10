#include "panels/log_displayer.hpp"

#include <imgui.h>
#include <glad/gl.h>

#include "debug.hpp"
#include "resource_manager.hpp"


LogDisplayer::LogDisplayer()
    : m_showedLogMask(1 << (int)ELogType::INFO | 1 << (int)ELogType::WARNING | 1 << (int)ELogType::ERROR)
{
    m_debug = Debug::GetInstance();

    ResourceManager* RM = ResourceManager::GetInstance();

    m_logTextures[0] = RM->AddResource<Texture>("Internal/IconsLog/info_icon.png", true, false, ETextureFormat::RGBA);
    m_logTextures[1] = RM->AddResource<Texture>("Internal/IconsLog/warning_icon.png", true, false, ETextureFormat::RGBA);
    m_logTextures[2] = RM->AddResource<Texture>("Internal/IconsLog/error_icon.png", true, false, ETextureFormat::RGBA);

    for (int i = 0; i < 3; ++i)
    {
        GenerateGPUTexture(m_logTextures[i]);
        GPUTextureLog* gpuTextureLog = static_cast<GPUTextureLog*>(m_logTextures[i]->m_gpuTexture2D.get());
        m_gpuTextureIDs[i] = reinterpret_cast<void*>((uintptr_t)gpuTextureLog->m_ID);
    }
}

void LogDisplayer::GenerateGPUTexture(std::shared_ptr<Texture> texture)
{
    std::unique_ptr<GPUTextureLog> gpuTexture = std::make_unique<GPUTextureLog>();

    glGenTextures(1, &gpuTexture->m_ID);
    glBindTexture(GL_TEXTURE_2D, gpuTexture->m_ID);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (!texture->GetData() || texture->GetWidth() <= 0 || texture->GetHeight() <= 0)
        return;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->GetWidth(), texture->GetHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, texture->GetData());
    glGenerateMipmap(GL_TEXTURE_2D);

    texture->m_gpuTexture2D = std::move(gpuTexture);
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

            if (ImGui::BeginTable("Log", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInner))
            {
                for (auto it = collapsedLogs->rbegin(); it != collapsedLogs->rend(); ++it)
                {
                    if (m_showedLogMask & 1 << (int)it->second.m_logType)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Image(m_gpuTextureIDs[(int)it->second.m_logType],
                            { 20.f, 20.f });

                        ImGui::SameLine();
                        ImGui::Text("[%i] %s",
                            it->second.m_count,
                            it->second.m_logMessage.c_str());
                    }
                }

                ImGui::EndTable();
            }

        }
        else
        {
            if (ImGui::BeginTable("Log", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInner))
            {
                std::vector<Log>* logs = m_debug->GetLogs();

                if (m_isAutoScrolling && m_debug->IsLogAdded())
                {
                    ImGui::SetScrollY(static_cast<float>(logs->size() * 25));
                    m_debug->ResetLogAddded();
                }
                
                int id = 0;
                for (auto it = logs->begin(); it != logs->end(); ++it)
                {
                    if (m_showedLogMask & 1 << (int)it->m_logMessage->m_logType)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);

                        ImGui::PushID(id);
                        if (it->m_isSelected = ImGui::TreeNode(""))
                        {
                            ImGui::TreePop();
                        }
                        ImGui::PopID();

                        ImGui::SameLine();

                        ImGui::Image(m_gpuTextureIDs[(int)it->m_logMessage->m_logType],
                            { 20.f, 20.f });

                        ImGui::SameLine();

                        ImGui::Text("[%02u:%02u:%02u] %s",
                            it->m_date.hours,
                            it->m_date.minutes,
                            it->m_date.seconds,
                            it->m_logMessage->m_logMessage.c_str());

                        if (it->m_isSelected)
                        {
                            ImGui::TextWrapped("Details : %s", it->m_logMessage->m_logMessage.c_str());
                            ImGui::TextWrapped("file %s(%i) : %s", 
                                it->file.c_str(),
                                it->line,
                                it->function.c_str());
                        }

                    }
                    ++id;
                }

                ImGui::EndTable();
            }
        }
    }

    ImGui::End();
}

void LogDisplayer::RenderMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        ImVec4 colorButton;
        ImVec4 colorButtonUp = { 1.f,1.f,1.f,0.2f };
        ImVec4 colorButtonDown = { 1.f,1.f,1.f,1.f };
        ImVec2 buttonSize = { 15.f,15.f };
        
        ImGui::Selectable("Clear on play", &m_isClearOnPlay, 0, ImGui::CalcTextSize("Clear on play"));
        ImGui::Spacing();
        ImGui::Selectable("Auto scroll", &m_isAutoScrolling, 0, ImGui::CalcTextSize("AutoScroll"));
        ImGui::Spacing();
        ImGui::Selectable("Collapse", &m_isCollapsing, 0, ImGui::CalcTextSize("Collapse"));

        const std::array<int, 3u>& logTypeCounts = m_debug->GetLogTypeCounts();

        for (unsigned int i = 0; i < 3u; i++)
        {
            unsigned char showedLogBit = 1 << i;
            bool shouldDisplay = m_showedLogMask & showedLogBit;

            colorButton = shouldDisplay ? colorButtonDown : colorButtonUp;

            std::string logCountStr = std::format("({}) ### {}", logTypeCounts[i], i);

            if (ImGui::ImageButtonWithText(m_gpuTextureIDs[i], logCountStr.c_str(), buttonSize, { 0,0 }, { 1,1 }, -1, { 0,0,0,0 }, colorButton))
            {
                if (shouldDisplay)
                    m_showedLogMask &= ~showedLogBit;
                else
                    m_showedLogMask |= showedLogBit;
            }
        }

        if (ImGui::Button("Clear")) Clear();
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
    if (m_currentDisplayedLog)
    {
        m_currentDisplayedLog->m_isSelected = false;
        m_currentDisplayedLog = nullptr;
    }

    // Add Clearing code
    if (m_isClearOnPlay)
        m_debug->Clear();
}
