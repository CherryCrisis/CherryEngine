#pragma once

#include "imgui_notify.h"
#include <filesystem>

#include <memory>
#include <CherryHeader.h>
#include "scene.hpp"

static const std::filesystem::path AssetPath = "../Assets";

class EditorManager 
{
public:
    std::shared_ptr<Scene> scene;
    EditorManager();
	void DisplayEditorUI(GLFWwindow* window);
    Engine* engine;
private:
    void HandleDocking();
    void HandleLogWindow();
    void HandleToolsWindow();
    void HandleBrowserWindow();
    void HandleGameWindow(unsigned int fbo = 0);
    void HandleEditorWindow(unsigned int fbo = 0);
    void HandleInspectorWindow();
    void HandleGraphWindow(GLFWwindow* window);
    void HandleNotifications();
    void HandleMenuBar();
    void SendNotifiction(const char* title, const char* content, ImGuiToastType_ type, float displayTime = 3.f);

    void QuerryBrowser();
    
    unsigned int GetIconByExtension(const char* ext);

    bool isDemoOpened = true;
    bool isLogOpened = true;
    bool isGameOpened = true;
    bool isSceneOpened = true;
    bool isHierarchyOpened = true;
    bool isInspectorOpened = true;
    bool isBrowserOpened = true;
    
    unsigned int browserIcon = 0;
    unsigned int FileIcon = 0;
    unsigned int PlayIcon = 0;
    unsigned int PauseIcon = 0;
    unsigned int ReplayIcon = 0;
    unsigned int StopIcon = 0;

    std::filesystem::path currentDirectory; 
};