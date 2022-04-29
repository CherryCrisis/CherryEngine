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

#include "maths.hpp"

// not sure about this
static const std::filesystem::path AssetPath = "Assets";
 
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

    //std::unordered_map<type, Behaviour*> GetSimilarComponents();

    void ApplyRange();

    Entity* m_startRange = nullptr;
    Entity* m_endRange = nullptr;
};

class EditorManager 
{
private:

    //Panel's Classes
    AssetBrowser             m_browser               {};
    LogDisplayer             m_logDisplayer          {};
    Inspector                m_inspector             {true, this};
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

    // TODO: To Replace with Resource Manager Texture Handling
    uint64_t PlayIcon = 0;
    uint64_t PauseIcon = 0;
    uint64_t ReplayIcon = 0;
    uint64_t StopIcon = 0;

    InputManager::InputContext* m_editorContext;
public:

    EntitySelector m_entitySelector;

    static void SendNotification(const char* title, ENotifType type, float displayTime = 3.f);
    
    EditorManager(const std::string& projectPath="");
    void DisplayEditorUI(GLFWwindow* window);

    void LinkEngine(Engine* engine);

    void FocusCallback(GLFWwindow* window, int focused);


    void FocusEntity(Entity* entity);

    static bool LoadTextureFromFile(const char* filename, uint64_t* out_texture, int* out_width, int* out_height);

    InputManager* inputs;

    //Selected Entities in hierarchy
    std::vector<Entity*> m_selectedEntities;

    std::string m_projectPath = "";
};

namespace EditorNotifications 
{
    void SceneSaving(bool result);
    void SceneLoading(bool result);
    void ObjectLoading(bool result);
    void ResourceImporting(bool result);
    void BuildGame(bool result);
}