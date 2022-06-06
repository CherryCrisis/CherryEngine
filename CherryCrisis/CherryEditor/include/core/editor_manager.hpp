#pragma once

#include <imgui_notify.h>
#include <filesystem>
#include <memory>

#include "cherry_header.hpp"
#include "maths.hpp"
#include "scene.hpp"
#include "panels/asset_browser.hpp"
#include "panels/asset_settings.hpp"
#include "panels/build_displayer.hpp"
#include "panels/cell_system_displayer.hpp"
#include "panels/game_displayer.hpp"
#include "panels/hierarchy_displayer.hpp"
#include "panels/inspector.hpp"
#include "panels/log_displayer.hpp"
#include "panels/preferences_displayer.hpp"
#include "panels/project_settings_displayer.hpp"
#include "panels/scene_displayer.hpp"
#include "panels/ui_editor.hpp"

 
struct GLFWwindow;
class Entity;

struct EntitySelector
{
    std::vector<Entity*> m_entities;

    Entity* m_startRange = nullptr;
    Entity* m_endRange = nullptr;

    bool Add(Entity* entity);
    bool Remove(Entity* entity, bool unselect = true);
    bool Clear(bool unselect = true);
    void ClearEx(); // Do not use
    bool IsEmpty() { return m_entities.size() <= 0; }
    bool Contains(Entity* entity);

    void SetEndRange();
    void SetStartRange(Entity* entity);
    int Count() const { return static_cast<int>(m_entities.size()); }

    void GetSimilarComponents();

    void ApplyRange();

    Entity* First() { return IsEmpty() ? nullptr : m_entities[0]; }
};

class EditorManager 
{
private:
    //Panel's Classes
    AssetSettingsDisplayer   m_assetSettingsDisplayer{};
    AssetBrowser             m_browser               {&m_assetSettingsDisplayer, this};
    BuildDisplayer           m_buildDisplayer        {false};
    CellSystemDisplayer      m_cellSystemDisplayer   {this};
    GameDisplayer            m_gameDisplayer         {this};
    HierarchyDisplayer       m_hierarchyDisplayer    {true, this};
    Inspector                m_inspector             {true, this, &m_assetSettingsDisplayer};
    LogDisplayer             m_logDisplayer          {};
    PreferencesDisplayer     m_preferencesDisplayer  {false};
    ProjectSettingsDisplayer m_projSettingsDisplayer {false};
    SceneDisplayer           m_sceneDisplayer        {};
    UIEditor                 m_uiEditor              {};
    //--------------

    bool    m_isDemoOpened     = false;
    bool    m_isFeaturerOpened = false;
    void*   m_gpuTextureIDs[4];

    std::shared_ptr<Texture> m_menuBarTextures[4];

    void HandleFeaturerWindow(GLFWwindow* window);
    void HandleNotifications();
    void HandleMenuBar();

    void UpdateFocusGame();

    void GenerateGPUTexture(std::shared_ptr<Texture> texture);

public:
    static void HandleDocking();
    
    float       m_menubarSize = 0.f;
    std::string m_projectPath = "";

    EntitySelector  m_entitySelector;

    Engine*                     m_engine = nullptr;
    InputManager::InputContext* m_editorContext = nullptr;

    EditorManager(const std::string& projectPath="");
    
    void LinkEngine(Engine* engine);

    void DisplayEditorUI(GLFWwindow* window);

    void FocusCallback(GLFWwindow* window, int focused);
    void FocusEntity(Entity* entity);

    void RefreshHierarchy() { m_hierarchyDisplayer.Refresh(); }
    void InvalidateCellDisplayer() { m_cellSystemDisplayer.InvalidatePointers(); }
    
    std::string             GetCompilerPath() { return m_preferencesDisplayer.GetCompilerPath(); }
    AssetBrowser*           GetAssetBrowser() { return &m_browser; }
    CellSystemDisplayer*    GetCellSystemDisplayer() { return &m_cellSystemDisplayer; }

    static void SendNotification(const char* title, ENotifType type, float displayTime = 3.f);

    void CheckForHierarchyRefresh();
    void SetPath() { m_browser.SetPath(m_projectPath); }
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
