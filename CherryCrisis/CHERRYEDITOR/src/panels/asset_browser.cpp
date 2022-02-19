
#include "panels/asset_browser.hpp"

#include <imgui.h>

static const std::filesystem::path AssetPath = "../Assets";

AssetBrowser::AssetBrowser() 
{
    m_current_directory = AssetPath;
}

void AssetBrowser::QuerryBrowser() 
{
    namespace fs = std::filesystem;

    float width = ImGui::GetContentRegionAvail().x;


    float cellSize = m_thumbnail_size + m_padding;
    int columnCount = (int)(width / cellSize);    if (columnCount < 1) columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    for (const fs::directory_entry& entry : fs::directory_iterator(m_current_directory))
    {

        unsigned int icon = entry.is_directory() ? m_browser_icon : m_file_icon;

        ImGui::ImageButton((void*)(intptr_t)icon, { m_thumbnail_size, m_thumbnail_size }, { 0,1 }, { 1, 0 });

        // Double Click Callback
        if (ImGui::IsItemHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) >= 2)
        {
            if (entry.is_directory())
                m_current_directory /= entry.path().filename();
        }

        ImGui::Text(entry.path().filename().string().c_str());

        ImGui::NextColumn();
    }
    ImGui::Columns(1);
}

void AssetBrowser::Render()
{
    if (!is_opened)
        return;


    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Browser", &is_opened, ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        if (m_current_directory != AssetPath)
            if (ImGui::Button("<-"))
                m_current_directory = m_current_directory.parent_path();


        std::filesystem::path path_iterator = m_current_directory;

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