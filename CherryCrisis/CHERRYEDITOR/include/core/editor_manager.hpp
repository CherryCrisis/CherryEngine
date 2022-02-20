#pragma once

#include "imgui_notify.h"
#include <filesystem>

#include <memory>
#include <CherryHeader.h>
#include "scene.hpp"

#include "panels/asset_browser.hpp"
#include "panels/log_displayer.hpp"
#include "panels/inspector.hpp"
#include "panels/game_displayer.hpp"
#include "panels/scene_displayer.hpp"
#include "panels/hierarchy_displayer.hpp"

static const std::filesystem::path AssetPath = "../Assets";
 
class EditorManager 
{
public:
    std::shared_ptr<Scene> scene;
    
    EditorManager();
	void DisplayEditorUI(GLFWwindow* window);

    void LinkEngine(Engine* engine);
private:

    //Panel's Classes
    AssetBrowser       m_browser            {};
    LogDisplayer       m_logDisplayer       {};
    Inspector          m_inspector          {};
    GameDisplayer      m_gameDisplayer      {};
    SceneDisplayer     m_sceneDisplayer     {};
    HierarchyDisplayer m_hierarchyDisplayer {};
    //--------------
    
    Engine* m_engine;

    unsigned int m_gameViewFBO = 0u;
    unsigned int m_gameViewRBO = 0u;
    unsigned int m_gameViewTex = 0u;

    void HandleDocking();
    void HandleGameWindow();
    void HandleEditorWindow(unsigned int fbo = 0);
    void HandleGraphWindow(GLFWwindow* window);
    void HandleNotifications();
    
    void HandleMenuBar();

    void SendNotifiction(const char* title, const char* content, ImGuiToastType_ type, float displayTime = 3.f);

    bool isDemoOpened = false;
    bool isGameOpened = true;
    bool isSceneOpened = true;
    bool isHierarchyOpened = true;

    unsigned int PlayIcon = 0;
    unsigned int PauseIcon = 0;
    unsigned int ReplayIcon = 0;
    unsigned int StopIcon = 0;
};