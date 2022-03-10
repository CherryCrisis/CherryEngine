
#include "panels/asset_browser.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <stdio.h>

#include <windows.h>
#undef far
#undef near
#include <ShellAPI.h>

#include "core/editor_manager.hpp"

AssetBrowser::AssetBrowser() 
{
    //Set Full Path to Assets
    m_currentDirectory = std::filesystem::current_path();
    m_currentDirectory /= AssetPath;
    int null = 0;

    if (!EditorManager::LoadTextureFromFile("internal/icons/file_icon.png", &m_fileIcon, &null, &null))
        std::cout << "failed to load file icon" << std::endl;

    if (!EditorManager::LoadTextureFromFile("internal/icons/folder_icon.png", &m_browserIcon, &null, &null))
        std::cout << "failed to load folder icon" << std::endl;

    // Load Icons
    QuerryBrowser();
}

void AssetBrowser::QuerryBrowser() 
{
    //To change, 
    m_nodes.clear();

    namespace fs = std::filesystem;
    int i = 0;
    for (const fs::directory_entry& entry : fs::directory_iterator(m_currentDirectory))
    {
        AssetNode node;
        node.m_isDirectory = entry.is_directory(); 
        node.m_icon = entry.is_directory() ? m_browserIcon : m_fileIcon;
        node.m_path = entry.path();
        node.m_filename = entry.path().filename().string();
        m_nodes[i] = node;
        i++;
    }
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
            {
                m_currentDirectory = m_currentDirectory.parent_path();
                QuerryBrowser();
            }
        
        std::filesystem::path path_iterator = m_currentDirectory.relative_path();
        std::filesystem::path path = AssetPath.parent_path();
        while (path_iterator != AssetPath.parent_path())
        {
            ImGui::SameLine(); ImGui::Text("/"); ImGui::SameLine();
            std::string name = path_iterator.filename().string();
            ImGui::Text(name.c_str());

            path_iterator = path_iterator.parent_path();
        }

        ContextCallback();

        if (ImGui::IsWindowHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Right) == 1)
        {
            ImGui::OpenPopup("context");

            CheckThings();
        }
        //TODO : Change sensibility in editor preferences
        if (InputManager::instance()->GetKey(Keycode::LEFT_CONTROL) && InputManager::instance()->GetMouseWheel().y != 0)
        {
            m_thumbnailSize += InputManager::instance()->GetMouseWheel().y  * 2;
        }

        RenderNodes();
    }
    ImGui::End();
}

void AssetBrowser::AssetNode::Render() 
{

}

void AssetBrowser::ContextCallback() 
{
    if (ImGui::BeginPopupContextItem("context"))
    {
        ImGui::Text("Actions ...");
        ImGui::Separator();
        if (ImGui::BeginMenu("New")) 
        {
            if (ImGui::MenuItem("Folder")) 
            {
                std::filesystem::path newPath = m_currentDirectory;
                newPath /= "New Folder";
                std::filesystem::create_directory(newPath);
                QuerryBrowser();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("C# Script"))   {}
            if (ImGui::MenuItem("Material"))    {}
            if (ImGui::MenuItem("Texture"))     {}
            if (ImGui::MenuItem("Scene"))       {}
            if (ImGui::MenuItem("Prefab"))      {}

            ImGui::EndMenu();
        }
        if (m_focusedNode) 
        {
            ImGui::Separator();
            if (ImGui::MenuItem("Open")) { ShellExecuteA(NULL, NULL, m_focusedNode->m_path.string().c_str(), NULL, NULL, 10); }
            if (ImGui::MenuItem("Rename")) {}
            std::string str = AssetPath.string();
            str = str + "/" + m_focusedNode->m_filename;
            if (ImGui::MenuItem("Delete")) 
            {
                remove(str.c_str()); 
                QuerryBrowser();
            }

            ImGui::Separator();
        }
        if (ImGui::MenuItem("Open In Explorer")) 
        {
            if (m_focusedNode)
            {
                std::string str = "/select,";
                str += m_focusedNode->m_path.string();
                
                ShellExecuteA(NULL, "open", "explorer.exe", str.c_str(), NULL, 10);
            }
            else
            {
                ShellExecuteA(NULL, NULL, m_currentDirectory.string().c_str() , NULL, NULL, 10);
            }
        }

        ImGui::EndPopup();
    }
}

void AssetBrowser::CheckThings()
{
    for (auto& couple : m_nodes)
    {
        AssetNode& node = couple.second;
        unsigned int id = ImGui::GetHoveredID();
        if (id == node.m_ImGuiID)
        {
            m_focusedNode = &couple.second;
            return;
        }
    }
    m_focusedNode = nullptr;
}


void AssetBrowser::RenderNodes() 
{
    float width = ImGui::GetContentRegionAvail().x;


    float cellSize = m_thumbnailSize + m_padding;
    int columnCount = (int)(width / cellSize);    if (columnCount < 1) columnCount = 1;

    ImGui::Columns(columnCount, 0, false);
    int i = 0;
    for (auto& couple : m_nodes) 
    {
        i++;
        AssetNode& node = couple.second;

        ImGui::PushID(i);
        ImGui::ImageButton((void*)(intptr_t)node.m_icon, { m_thumbnailSize, m_thumbnailSize }, { 0,1 }, { 1, 0 });

        node.m_ImGuiID = ImGui::GetItemID();
        
        ImGui::PopID();
        // Double Click Callback
        if (ImGui::IsItemHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) >= 2)
        {
            if (node.m_isDirectory)
            {
                m_currentDirectory /= node.m_filename;
                QuerryBrowser();
                return;
            }
        }

        // Need to set double click callback rename
        ImGui::Text(node.m_filename.c_str());

        if (ImGui::IsItemHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) == 2) 
        {
            EditorManager::SendNotification("Outch it hurts", ImGuiToastType_Warning);
        }

        ImGui::NextColumn();
    }
    ImGui::Columns(1);
}