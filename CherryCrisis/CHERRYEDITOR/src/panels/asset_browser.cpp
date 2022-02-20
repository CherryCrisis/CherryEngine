
#include "panels/asset_browser.hpp"

#include <imgui.h>

static const std::filesystem::path AssetPath = "../Assets";

AssetBrowser::AssetBrowser() 
{
    m_currentDirectory = AssetPath;
}

void AssetBrowser::QuerryBrowser() 
{
    namespace fs = std::filesystem;

    float width = ImGui::GetContentRegionAvail().x;


    float cellSize = m_thumbnailSize + m_padding;
    int columnCount = (int)(width / cellSize);    if (columnCount < 1) columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    for (const fs::directory_entry& entry : fs::directory_iterator(m_currentDirectory))
    {

        unsigned int icon = entry.is_directory() ? m_browserIcon : m_fileIcon;

        ImGui::ImageButton((void*)(intptr_t)icon, { m_thumbnailSize, m_thumbnailSize }, { 0,1 }, { 1, 0 });

        // Double Click Callback
        if (ImGui::IsItemHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) >= 2)
        {
            if (entry.is_directory())
                m_currentDirectory /= entry.path().filename();
        }

        ImGui::Text(entry.path().filename().string().c_str());

        ImGui::NextColumn();
    }
    ImGui::Columns(1);
}

void AssetBrowser::Render()
{
    if (!m_isOpened)
        return;


    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Browser", &m_isOpened, ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        if (m_currentDirectory != AssetPath)
            if (ImGui::Button("<-"))
                m_currentDirectory = m_currentDirectory.parent_path();


        std::filesystem::path path_iterator = m_currentDirectory;

        while (path_iterator != AssetPath.parent_path())
        {
            ImGui::SameLine(); ImGui::Text("/"); ImGui::SameLine();
            std::string name = path_iterator.filename().string();
            ImGui::Text(name.c_str());

            path_iterator = path_iterator.parent_path();
        }

        QuerryBrowser();
    }
    ImGui::End();
}