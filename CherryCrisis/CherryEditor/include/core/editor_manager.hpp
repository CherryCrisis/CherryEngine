#pragma once

#include "imgui_notify.h"
#include <filesystem>

#include <memory>
#include "cherry_header.hpp"
#include "scene.hpp"

#include "panels/asset_browser.hpp"
#include "panels/log_displayer.hpp"
#include "panels/inspector.hpp"
#include "panels/game_displayer.hpp"
#include "panels/scene_displayer.hpp"
#include "panels/hierarchy_displayer.hpp"
#include "panels/preferences_displayer.hpp"
#include "panels/project_settings_displayer.hpp"
#include "panels/build_displayer.hpp"

// not sure about this
static const std::filesystem::path AssetPath = "Assets";
 
class GLFWwindow;
class EditorManager 
{
private:

    //Panel's Classes
    AssetBrowser             m_browser               {};
    LogDisplayer             m_logDisplayer          {};
    Inspector                m_inspector             {};
    GameDisplayer            m_gameDisplayer         {};
    SceneDisplayer           m_sceneDisplayer        {};
    HierarchyDisplayer       m_hierarchyDisplayer    {};
    PreferencesDisplayer     m_preferencesDisplayer  {false};
    ProjectSettingsDisplayer m_projSettingsDisplayer {false};
    BuildDisplayer           m_buildDisplayer        {false};
    //--------------
    
    Engine* m_engine = nullptr;

    void HandleDocking();
    void HandleFeaturerWindow(GLFWwindow* window);
    void HandleNotifications();
    
    void HandleMenuBar();


    bool m_isDemoOpened     = false;
    bool m_isFeaturerOpened = false;

    // To Replace with Resource Manager Texture Handling
    unsigned int PlayIcon = 0;
    unsigned int PauseIcon = 0;
    unsigned int ReplayIcon = 0;
    unsigned int StopIcon = 0;

public:
    // Temporary, to replace with scene engine handling
    std::shared_ptr<Scene> scene;

    static void SendNotification(const char* title, ImGuiToastType_ type, float displayTime = 3.f);
    
    EditorManager();
    void DisplayEditorUI(GLFWwindow* window);

    void LinkEngine(Engine* engine);

    void FocusCallback(GLFWwindow* window, int focused);

    InputManager* inputs;
};