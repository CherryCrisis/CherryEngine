
#include "core/editor_manager.hpp"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <string>
#include <iostream>
#include <filesystem>

#include <imgui_impl_opengl3.h>
#include "ImGuizmo.h"

#include <glad/gl.h>
#include <tchar.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 

#include "resource_manager.hpp"
#include "scene_manager.hpp"
#include "render_manager.hpp"
#include "csscripting_system.hpp"

#include "builder.hpp"
#include "command.hpp"

EditorManager::EditorManager(const std::string& projectPath) 
{
    ResourceManager* RM = ResourceManager::GetInstance();

    m_menuBarTextures[0] = RM->AddResource<Texture>("Internal/Icons/play_icon.png", true, false, ETextureFormat::RGBA);
    m_menuBarTextures[1] = RM->AddResource<Texture>("Internal/Icons/pause_icon.png", true, false, ETextureFormat::RGBA);
    m_menuBarTextures[2] = RM->AddResource<Texture>("Internal/Icons/replay_icon.png", true, false, ETextureFormat::RGBA);
    m_menuBarTextures[3] = RM->AddResource<Texture>("Internal/Icons/stop_icon.png", true, false, ETextureFormat::RGBA);

    for (int i = 0; i < 4; ++i)
    {
        GenerateGPUTexture(m_menuBarTextures[i]);
        GPUTextureLog* gpuTextureLog = static_cast<GPUTextureLog*>(m_menuBarTextures[i]->m_gpuTexture2D.get());
        m_gpuTextureIDs[i] = reinterpret_cast<void*>((uintptr_t)gpuTextureLog->m_ID);
    }

    m_buildDisplayer.projectSettings = &m_projSettingsDisplayer;
    m_cellSystemDisplayer.m_camera = &m_sceneDisplayer.m_camera;

    m_projectPath = projectPath.size() > 0 ? projectPath : std::filesystem::current_path().filename().string();
    m_browser.SetPath(m_projectPath);

    Serializer::UnserializeEditor("editor.meta");

    
}

void EditorManager::GenerateGPUTexture(std::shared_ptr<Texture> texture)
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

void EditorManager::LinkEngine(Engine* engine) 
{
    m_engine = engine;
    m_inspector.m_engine = engine;
    m_sceneDisplayer.m_manager = this;
}

void EditorManager::DisplayEditorUI(GLFWwindow* window) 
{
    HandleDocking();

    if (!m_entitySelector.IsEmpty()) ImGuizmo::Enable(true);

    HandleMenuBar();
    m_browser.Render();
    m_assetSettingsDisplayer.Render();
    m_logDisplayer.Render();
    m_sceneDisplayer.Render();
    m_inspector.Render();
    m_gameDisplayer.Render();
    m_cellSystemDisplayer.Render();
    m_hierarchyDisplayer.Render();
    m_preferencesDisplayer.Render();
    m_projSettingsDisplayer.Render();
    m_buildDisplayer.Render();
    HandleFeaturerWindow(window);

    HandleNotifications();

    UpdateFocusGame();

    if (m_isDemoOpened)   ImGui::ShowDemoWindow(&m_isDemoOpened);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorManager::HandleDocking() 
{
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Docking", NULL, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    ImGui::End();
}

void EditorManager::HandleMenuBar() 
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save", "Ctrl+S")) 
                EditorNotifications::SceneSaving(SceneManager::SaveCurrentScene());

            if (ImGui::MenuItem("Save As..")) {}


            if (ImGui::MenuItem("Launch"))
            {
                call("open","CherryStandalone.exe");
            }


            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Project Settings")) { m_projSettingsDisplayer.Toggle(true); }
            if (ImGui::MenuItem("Preferences"))      { m_preferencesDisplayer.Toggle(true); }
            ImGui::Separator();
            if (ImGui::MenuItem("Build")) { m_buildDisplayer.Toggle(true); } // Open Build Menu 
            if (ImGui::MenuItem("Build And Run")) 
            {
                Builder::BuildGame(m_buildDisplayer.outDir, m_projSettingsDisplayer.GetBuildSettings().m_gameName.c_str(), true);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows"))
        {
            if (ImGui::BeginMenu("Open"))
            {
                if (ImGui::MenuItem("Browser")) { m_browser.Toggle(true); }
                if (ImGui::MenuItem("Hierarchy")) { m_hierarchyDisplayer.Toggle(true); }
                if (ImGui::MenuItem("Log"))       { m_logDisplayer.Toggle(true); }
                if (ImGui::MenuItem("Inspector")) { m_inspector.Toggle(true); }
                if (ImGui::MenuItem("Asset Settings")) { m_assetSettingsDisplayer.Toggle(true); }
                if (ImGui::MenuItem("Game"))      { m_gameDisplayer.Toggle(true); }
                if (ImGui::MenuItem("Scene"))     { m_sceneDisplayer.Toggle(true); }
                if (ImGui::MenuItem("Featurer"))  { m_isFeaturerOpened = true; }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }


        float size = ImGui::GetWindowHeight() - 4.f;
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * .5f) - (size * .5f * 3.f));

        // Start or Stop game in editor
        if (ImGui::ImageButton(m_engine->m_isPlaying ? (ImTextureID)m_gpuTextureIDs[3] : (ImTextureID)m_gpuTextureIDs[0], ImVec2(size, size), { 0,0 }, { 1,1 }, -1))
        {
            // Stop game if playing
            if (m_engine->m_isPlaying)
            {
                m_engine->Stop();
                m_entitySelector.Clear();
                m_gameDisplayer.Unfocus();
            }

            // Start game if not playing
            else
            {
                m_logDisplayer.TryClearOnPlay();
                m_engine->Launch();
                m_gameDisplayer.Focus();
            }

        } ImGui::SameLine();

        // Pause game in editor
        if (ImGui::ImageButton((ImTextureID)m_gpuTextureIDs[1], ImVec2(size, size), { 0,0 }, { 1,1 }, -1, m_engine->m_isPaused ? ImVec4(1.f, 1.f, 1.f, 0.3f) : ImVec4(0.f, 0.f, 0.f, 0.f)))
        {
            if (m_engine->m_isPlaying)
                m_engine->m_isPaused = !m_engine->m_isPaused;

        } ImGui::SameLine();
        
        // Restart game in editor
        if (ImGui::ImageButton((ImTextureID)m_gpuTextureIDs[2], ImVec2(size, size), { 0,0 }, { 1,1 }, -1))
        {
            // Stop game
            m_engine->Stop();
            m_entitySelector.Clear();

            // Launch game
            m_logDisplayer.TryClearOnPlay();
            m_engine->Launch(false);
            m_gameDisplayer.Focus();
        }
        ImGui::PopStyleColor(1);
        ImGui::EndMainMenuBar();
    }
}

void EditorManager::HandleFeaturerWindow(GLFWwindow* window)
{
    if (!m_isFeaturerOpened)
        return;

    if (ImGui::Begin("Featurer", &m_isFeaturerOpened))
    {
        if (ImGui::Button("Close.."))
            ImGui::OpenPopup("Close?");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Close?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Files are unsaved!\n\n");
            ImGui::Separator();

            if (ImGui::Button("Close Anyway", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup(); glfwSetWindowShouldClose(window, true);
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        if (ImGui::Button("Success"))
            SendNotification("I am the Title ! %s", ENotifType::Success, 3.f);
        ImGui::SameLine();
        if (ImGui::Button("Warning"))
            SendNotification("I am the Title ! %s", ENotifType::Warning, 3.f);
        ImGui::SameLine();
        if (ImGui::Button("Error"))
            SendNotification("I am the Title ! %s", ENotifType::Error, 3.f);
        ImGui::SameLine();
        if (ImGui::Button("Info"))
            SendNotification("I am the Title ! %s", ENotifType::Info, 3.f);
        ImGui::SameLine();
        if (ImGui::Button("None"))
            SendNotification("I am the Title ! %s", ENotifType::None, 3.f);
        ImGui::SameLine();

        if (ImGui::Button("Show Demo"))
            m_isDemoOpened = true;
        
    }
    ImGui::End();
}

void EditorManager::HandleNotifications()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f));
    ImGui::RenderNotifications(); 
    ImGui::PopStyleVar(1); 
    ImGui::PopStyleColor(1);
}

void EditorManager::UpdateFocusGame()
{
    if (!m_gameDisplayer.m_isFocused)
    {
        InputManager::PushContext("Editor Context");
        if (m_gameDisplayer.m_isHovered && m_engine->m_isPlaying && InputManager::GetKeyDown(Keycode::LEFT_CLICK))
            m_gameDisplayer.Focus();
        InputManager::PopContext();
    }
    else if (m_gameDisplayer.m_isFocused && m_engine->m_isPlaying)
    {
        InputManager::PushContext("User Context");

        if (InputManager::GetKeyDown(Keycode::ESCAPE))
        {
            m_gameDisplayer.Unfocus();
        }
        InputManager::PopContext();
    }
}

//Display time in seconds
void EditorManager::SendNotification(const char* title, ENotifType type, float displayTime)
{
    ImGui::InsertNotification({ type, displayTime, title, ""});
}

void EditorManager::FocusCallback(GLFWwindow* window, int focused)
{
    m_browser.QuerryBrowser();
}

void EditorManager::FocusEntity(Entity* entity)
{
    m_entitySelector.Clear();
    m_entitySelector.Add(entity);
}

namespace EditorNotifications
{
    void SceneSaving(bool result) 
    {
        if (result)
            EditorManager::SendNotification("Scene Saved!", ENotifType::Success);
        else
            EditorManager::SendNotification("Scene Failed to save", ENotifType::Error);
    }

    void SceneLoading(bool result)
    {
        if (result)
            EditorManager::SendNotification("Scene Loaded!", ENotifType::Success);
        else
            EditorManager::SendNotification("Scene Failed to load", ENotifType::Error);
    }

    void ObjectLoading(bool result)
    {
        if (result)
            EditorManager::SendNotification("loading Object", ENotifType::Info);
        else
            EditorManager::SendNotification("Error while loading object", ENotifType::Warning);
    }

    void ResourceImporting(bool result)
    {
        if (result)
            EditorManager::SendNotification("importing Asset", ENotifType::Success);
        else
            EditorManager::SendNotification("Error while importing the asset", ENotifType::Warning);
    }

    void BuildGame(bool result) 
    {
        if (result)
            EditorManager::SendNotification("Building game..", ENotifType::Info);
        else
            EditorManager::SendNotification("Error while building game, see console", ENotifType::Error);
    }
}

bool EntitySelector::Add(Entity* entity) 
{
    if (std::count(m_entities.begin(), m_entities.end(), entity))
        return false;

    m_entities.push_back(entity);
    return true;
}

bool EntitySelector::Remove(Entity* entity) 
{
    for (auto it = m_entities.begin(); it != m_entities.end(); it++)
    {
        if (*it == entity)
        {
            m_entities.erase(it);
            break;
        }
    }

    return Contains(entity);
}

bool EntitySelector::Clear() 
{
    m_entities.clear();
    return m_entities.size() <= 0;
}

bool EntitySelector::Contains(Entity* entity) 
{
    return std::count(m_entities.begin(), m_entities.end(), entity);
}

void EntitySelector::SetStartRange(Entity* entity) 
{
    m_startRange = entity;
}

void EntitySelector::SetEndRange() 
{
    m_endRange = m_startRange;
}

void EntitySelector::ApplyRange() 
{
    if (!m_endRange)
        return;

    std::vector<Entity*>& entities = SceneManager::GetInstance()->m_currentScene->m_entities;

    auto startIt = std::find(entities.begin(), entities.end(), m_startRange);
    auto endIt   = std::find(entities.begin(), entities.end(), m_endRange);
    
     if (startIt - entities.begin() > endIt - entities.begin())
        for (auto it = endIt; it != startIt+1; it++)
            Add(*it);
    else
        for (auto it = startIt; it != endIt+1; it++)
            Add(*it);
    
    m_endRange = nullptr;
}

void EntitySelector::GetSimilarComponents() 
{
    for (Entity* entity : m_entities)
    {
        
    }
}