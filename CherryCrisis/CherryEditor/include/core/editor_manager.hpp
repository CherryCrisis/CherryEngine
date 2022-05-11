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
#include "panels/cell_system_displayer.hpp"
#include "panels/project_settings_displayer.hpp"
#include "panels/build_displayer.hpp"
#include "panels/asset_settings.hpp"

#include "maths.hpp"
 
struct GLFWwindow;
class Entity;

struct EntitySelector
{
    std::vector<Entity*> m_entities;

    bool Add(Entity* entity);
    bool Remove(Entity* entity);
    bool Clear();
    bool IsEmpty() { return m_entities.size() <= 0; }
    bool Contains(Entity* entity);

    void SetEndRange();
    void SetStartRange(Entity* entity);
    int Count() const { return static_cast<int>(m_entities.size()); }

    void GetSimilarComponents();

    void ApplyRange();

    Entity* First() { return IsEmpty() ? nullptr : m_entities[0]; }

    Entity* m_startRange = nullptr;
    Entity* m_endRange = nullptr;
};

class EditorManager 
{
private:

    //Panel's Classes
    AssetSettingsDisplayer   m_assetSettingsDisplayer{};
    AssetBrowser             m_browser               {&m_assetSettingsDisplayer};
    LogDisplayer             m_logDisplayer          {};
    Inspector                m_inspector             {true, this, &m_assetSettingsDisplayer};
    GameDisplayer            m_gameDisplayer         {};
    SceneDisplayer           m_sceneDisplayer        {};
    CellSystemDisplayer      m_cellSystemDisplayer   {};
    HierarchyDisplayer       m_hierarchyDisplayer    {true, this};
    PreferencesDisplayer     m_preferencesDisplayer  {false};
    ProjectSettingsDisplayer m_projSettingsDisplayer {false};
    BuildDisplayer           m_buildDisplayer        {false};
    //--------------

    Engine* m_engine = nullptr;

    void HandleDocking();
    void HandleFeaturerWindow(GLFWwindow* window);
    void HandleNotifications();
    
    void HandleMenuBar();

    void UpdateFocusGame();

    bool m_isDemoOpened     = false;
    bool m_isFeaturerOpened = false;

    std::shared_ptr<Texture> m_menuBarTextures[4];
    void* m_gpuTextureIDs[4];

    void GenerateGPUTexture(std::shared_ptr<Texture> texture);
public:

    EntitySelector m_entitySelector;

    static void SendNotification(const char* title, ENotifType type, float displayTime = 3.f);
    
    EditorManager(const std::string& projectPath="");
    
    AssetBrowser* GetAssetBrowser() { return &m_browser; }

    void DisplayEditorUI(GLFWwindow* window);

    void LinkEngine(Engine* engine);

    void FocusCallback(GLFWwindow* window, int focused);
    void FocusEntity(Entity* entity);

    InputManager* inputs;
    InputManager::InputContext* m_editorContext;

    std::string m_projectPath = "";
};

// Wrapper for generic notifications
namespace EditorNotifications 
{
    void SceneSaving(bool result);
    void SceneLoading(bool result);
    void ObjectLoading(bool result);
    void ResourceImporting(bool result);
    void BuildGame(bool result);
}