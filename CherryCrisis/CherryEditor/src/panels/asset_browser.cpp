#include "panels/asset_browser.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#include <iostream>

#include "core/editor_manager.hpp"
#include "command.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

#include "scene_manager.hpp"
#include "utils.hpp"

const char* scriptTemplate = 
R"CS(using CCEngine;

namespace CCScripting
{
    public class %s : Behaviour
    {
        public %s(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}


        //called at the start of the game
        public void Start()
        {

        }

        //called each tick 
        public void Update()
        {

        }
    }
})CS";

void replaceAll(std::string& str, const std::string& from, const std::string& to) 
{
    if (from.empty())
        return;

    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) 
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

AssetBrowser::AssetBrowser()
{
    //Set Full Path to Assets
    m_currentDirectory = std::filesystem::current_path();
    m_currentDirectory /= "Assets";
    m_solutionDirectory = m_currentDirectory;

    int null = 0;

    if (!EditorManager::LoadTextureFromFile("Internal/Icons/file_icon.png", &m_fileIcon, &null, &null))
        std::cout << "failed to load file icon" << std::endl;
     
    if (!EditorManager::LoadTextureFromFile("Internal/Icons/folder_icon.png", &m_browserIcon, &null, &null))
        std::cout << "failed to load folder icon" << std::endl;

    // Load Icons
    QuerryBrowser();
}

void AssetBrowser::QuerryBrowser()
{
    m_nodes.clear();

    if (std::filesystem::exists(m_currentDirectory)) 
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(m_currentDirectory)) 
            GenerateNode(entry);
}

void AssetBrowser::Render()
{
    if (!m_isOpened)
        return;

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Browser", &m_isOpened, ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        // Change this into an interactable directory displayer
        //------------------------------------------------------------------------------------------------------
        if (m_currentDirectory != m_solutionDirectory)
            if (ImGui::Button("<-"))
            {
                m_currentDirectory = m_currentDirectory.parent_path();
                QuerryBrowser();
            }
        ImGui::SameLine();
        ImGui::Text(m_currentDirectory.string().c_str());
        /*std::filesystem::path path_iterator = m_currentDirectory;
        while (path_iterator != m_solutionDirectory.parent_path())
        {
            ImGui::SameLine(); ImGui::Text("/"); ImGui::SameLine();
            std::string name = path_iterator.filename().string();
            ImGui::Text(name.c_str());

            path_iterator = path_iterator.parent_path();
        }*/
        //------------------------------------------------------------------------------------------------------
        ContextCallback();

        if (ImGui::IsWindowHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Right) == 1)
        {
            ImGui::OpenPopup("context");

            CheckThings();
        }
        //TODO : Change sensibility in editor preferences
        if (InputManager::GetInstance()->GetKey(Keycode::LEFT_CONTROL) && InputManager::GetInstance()->GetMouseWheel().y != 0)
        {
            m_thumbnailSize += InputManager::GetInstance()->GetMouseWheel().y * 2;
        }

        RenderNodes();
        RenderAssetsSettings();
    }
    ImGui::End();
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
                m_renaming = true;
                m_focusedNode = GetNodeByPath(newPath);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("C# Script")) m_creating = true;
            if (ImGui::MenuItem("Scene")) 
            {
                std::ofstream myfile;

                myfile.open("Assets/NewScene.cherry");
                if (myfile.is_open())
                {
                    myfile << " ";
                    myfile.close();
                }


                QuerryBrowser();
                m_focusedNode = GetNodeByName("NewScene");
                m_renaming = true;
            }


            ImGui::EndMenu();
        }
        if (m_focusedNode)
        {
            ImGui::Separator();

            if (ImGui::MenuItem("Open"))    call(0, m_focusedNode->GetFullPath().c_str());
            if (ImGui::MenuItem("Rename"))  m_renaming = true;
            if (ImGui::MenuItem("Delete"))  m_deleting = true;

            ImGui::Separator();
        }
        if (ImGui::MenuItem("Open In Explorer"))
        {
            if (m_focusedNode)
            {
                std::string str = "/select,";
                str += m_focusedNode->GetFullPath();

                call("open", "explorer.exe", str.c_str());
            }
            else
                call(0, m_currentDirectory.string().c_str());

        }

        ImGui::EndPopup();
    }
}

std::string AssetBrowser::AssetNode::GetFullPath() 
{
    return m_path.string() + "\\" + m_filename + m_extension;
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
}

void AssetBrowser::GenerateNode(const std::filesystem::directory_entry& entry)
{
    AssetNode node;

    node.m_isDirectory  = entry.is_directory();
    node.m_icon         = entry.is_directory() ? m_browserIcon : m_fileIcon; // To Change
    node.m_path         = entry.path().parent_path();
    node.m_relativePath = entry.path();
     
    std::string filename = entry.path().filename().string();
    node.m_filename = String::ExtractKey(filename, '.');

    node.m_extension = entry.path().extension().string();

    m_nodes[node.m_relativePath.string()] = node;
}

AssetBrowser::AssetNode* AssetBrowser::GetNodeByPath(std::filesystem::path path)
{
    for (auto& couple : m_nodes)
    {
        AssetNode& node = couple.second;
        if (path == node.m_path)
        {
            return &node;
        }
    }
    return nullptr;
}

AssetBrowser::AssetNode* AssetBrowser::GetNodeByName(const std::string& name)
{
    for (auto& couple : m_nodes)
    {
        AssetNode& node = couple.second;
        if (name == node.m_filename)
        {
            return &node;
        }
    }
    return nullptr;
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

        std::string name = node.m_filename + node.m_extension;
        const char* path = name.c_str();

        if (ImGui::BeginDragDropSource()) 
        {
            size_t length = name.size();
            ImGui::SetDragDropPayload(node.m_extension.c_str(), path,length+1, ImGuiCond_Once);
            ImGui::Text(path);
            ImGui::EndDragDropSource();
        }

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
            if (node.m_extension == ".cherry") 
            {
                //open Scene and if the last is not saved, warn the user
                std::string str = "Assets/"+node.m_filename;
                EditorNotifications::SceneLoading(SceneManager::LoadScene(str.c_str()));
            }
            if (node.m_extension.compare(".jpg") || node.m_extension.compare(".png"))
            {
                std::string str = "Assets/" + node.m_filename + node.m_extension;
                m_assetsSettings = std::unique_ptr<AssetsSettings>(new TextureSettings(str));
            }
        }

        ImGui::Text(path);

        if (ImGui::IsItemHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) == 2)
        {
            m_focusedNode = &node;
            m_renaming = true;
        }

        ImGui::NextColumn();
    }
    ImGui::Columns(1);

    // Create a class Popup 
    if (m_deleting)
        ImGui::OpenPopup("Delete");

    if (m_renaming)
        ImGui::OpenPopup("Rename");

    if (m_creating)
        ImGui::OpenPopup("Create Script");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Delete", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        std::string str = "Are you sure to delete ";
        str += m_focusedNode->m_filename;
        str += " ? \n\n";
        ImGui::Text("(IRREVERSIBLE!)");
        ImGui::Text(str.c_str());
        ImGui::Separator();

        if (ImGui::Button("Delete", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            std::string str = AssetPath.string();
            str = str + "/" + m_focusedNode->m_filename;
            if (m_focusedNode->m_isDirectory)
                std::filesystem::remove_all(str.c_str());
            else
                remove(str.c_str());
            QuerryBrowser();
            m_deleting = false;
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { m_deleting = false;  ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        std::string str = "Renaming ";
        str += m_focusedNode->m_filename;
        str += " ? \n\n";
        ImGui::Text(str.c_str());
        ImGui::Separator();

        static char newName[32] = "New Name";
        IMGUI_LEFT_LABEL(ImGui::InputText, "New Name:", newName, IM_ARRAYSIZE(newName));

        ImGui::Separator();

        if (ImGui::Button("Rename", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            std::filesystem::path newPath = m_focusedNode->m_path;
            newPath /= newName;
            if (!newPath.has_extension())
                newPath += m_focusedNode->m_extension;
            
            rename(m_focusedNode->GetFullPath().c_str(), newPath.string().c_str());
            QuerryBrowser();

            m_renaming = false;
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { m_renaming = false;  ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Create Script", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Name the new Script");
        ImGui::Separator();

        static char newName[32] = "New Name";
        IMGUI_LEFT_LABEL(ImGui::InputText, "New Name:", newName, IM_ARRAYSIZE(newName));

        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            std::ofstream myfile;
            std::string fileName = "Assets/"+std::string(newName)+".cs";

            bool opened = false;
            myfile.open(fileName);
            std::string str = scriptTemplate;
            replaceAll(str, "%s", newName);
            if (myfile.is_open())
            {
                myfile << str;
                myfile.close();

                EditorManager::SendNotification("Script Created", ENotifType::Success);
            }
            QuerryBrowser();

            m_creating = false;
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { m_creating = false;  ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}

void AssetBrowser::RenderAssetsSettings()
{
    if (m_assetsSettings)
    {
        if (!m_assetsSettings.get()->Update())
        {
            m_assetsSettings.reset();
        }
    }
}