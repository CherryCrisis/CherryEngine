
#include "panels/project_settings_displayer.hpp"

#include <imgui.h>

#include "core/editor_manager.hpp"
#include "resource_manager.hpp"
#include "serializer.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

ProjectSettingsDisplayer::ProjectSettingsDisplayer(bool spawnOpened = true) : Panel(spawnOpened)
{
    m_categories[0] = new General("Generals");
    m_categories[1] = new Audio("Audio");
    m_categories[2] = new Physics("Physics");
    m_categories[3] = new Input("Inputs");
    m_categories[4] = new TagLayer("Tags / Layers");
    m_categories[5] = new RenderPass("Render Pass");
    m_categories[6] = new ResourceViewer("Resource Viewer");
}

ProjectSettingsDisplayer::~ProjectSettingsDisplayer()
{
    for (PanelCategory* category : m_categories)
    {
        delete category;
    }
}

void ProjectSettingsDisplayer::Render()
{
    if (!m_isOpened)
        return;

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(.5f, .5f));

    if (ImGui::Begin("Project Settings", &m_isOpened, ImGuiWindowFlags_MenuBar))
    {
        ImGui::BeginMenuBar();
        static char name[32] = "";
        ImGui::InputText("Search", name, IM_ARRAYSIZE(name));
        ImGui::EndMenuBar();

        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            ImGui::BeginChild("CategoryList", ImVec2(ImGui::GetContentRegionAvail().x * 0.25f, ImGui::GetContentRegionAvail().y), false, window_flags);

            int i = 0;
            for (PanelCategory* category : m_categories)
            {
                if (category->DisplayLabel())
                    m_selectedCategory = i;

                i++;
            }

            ImGui::EndChild();
        }
        ImGui::SameLine();

        m_categories[m_selectedCategory]->DisplayCategory();

    }
    ImGui::End();
}


void ProjectSettingsDisplayer::General::Fill()
{
    IMGUI_LEFT_LABEL(ImGui::InputText, "Game Name :", name, IM_ARRAYSIZE(name));

    static char version[32] = "0.0.1";
    IMGUI_LEFT_LABEL(ImGui::InputText, "Version :", version, IM_ARRAYSIZE(version));

    static char company[32] = "Cherry";
    IMGUI_LEFT_LABEL(ImGui::InputText, "Company :", company, IM_ARRAYSIZE(company));
}

ProjectSettingsDisplayer::Input::Input(std::string name) : PanelCategory(name)
{
    userContext = InputManager::GetInstance()->GetOrAddContext("User Context");
}

void ProjectSettingsDisplayer::Input::Fill()
{
    InputManager* IM = InputManager::GetInstance();

    if (ImGui::Button("Save"))
        Serializer::SerializeInputs();

    static int type = -1;
    if (ImGui::Button("Add Button Mapped Action"))
    {
        ImGui::OpenPopup("Action Buttons Creation");
        type = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Axis Mapped Action"))
    {
        ImGui::OpenPopup("Action Axes Creation");
        type = 1;
    }

    CreateAction(IM, type);

    SetButtons(IM);
    ImGui::Spacing(); ImGui::Separator();
    SetAxes(IM);
}

void ProjectSettingsDisplayer::Input::CreateAction(InputManager* IM, int& type)
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Action Buttons Creation", NULL, ImGuiWindowFlags_AlwaysAutoResize) ||
        ImGui::BeginPopupModal("Action Axes Creation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        std::string str = "Name the new Action: \n\n";
        ImGui::Text(str.c_str());
        ImGui::Separator();

        static char name[32];
        IMGUI_LEFT_LABEL(ImGui::InputText, "Name:", name, IM_ARRAYSIZE(name));

        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();

            if (type == 0)
                CreateButtons(IM, name);
            else if (type == 1)
                CreateAxes(IM, name);

            type = -1;

            memset(name, 0, IM_ARRAYSIZE(name));
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}

void ProjectSettingsDisplayer::Input::CreateButtons(InputManager* IM, const char* name)
{
    int success = 0;
    IM->PushContext(userContext);
    IM->AddActionSingle(std::string(name), success);
    IM->PopContext();

    if (success == 1)
    {
        std::string notif = "Action Buttons " + std::string(name) + " created.";
        EditorManager::SendNotification(notif.c_str(), ENotifType::Success, 1.0f);
    }
    else if (success == 0)
    {
        std::string notif = "Action Buttons " + std::string(name) + " already exists.";
        EditorManager::SendNotification(notif.c_str(), ENotifType::Warning, 1.0f);
    }
    else if (success == -1)
    {
        std::string notif = "Action Buttons " + std::string(name) + " couldn't be created.";
        EditorManager::SendNotification(notif.c_str(), ENotifType::Error, 1.0f);
    }
}

void ProjectSettingsDisplayer::Input::CreateAxes(InputManager* IM, const char* name)
{
    int success = 0;
    IM->PushContext(userContext);
    IM->AddActionAxes(std::string(name), success);
    IM->PopContext();

    if (success == 1)
    {
        std::string notif = "Action Axes " + std::string(name) + " created.";
        EditorManager::SendNotification(notif.c_str(), ENotifType::Success, 1.0f);
    }
    else if (success == 0)
    {
        std::string notif = "Action Axes " + std::string(name) + " already exists.";
        EditorManager::SendNotification(notif.c_str(), ENotifType::Warning, 1.0f);
    }
    else if (success == -1)
    {
        std::string notif = "Action Axes " + std::string(name) + " couldn't be created.";
        EditorManager::SendNotification(notif.c_str(), ENotifType::Error, 1.0f);
    }
}

void ProjectSettingsDisplayer::Input::SetButtons(InputManager* IM)
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    int i = 0, j = 0;

    // Loop on Buttons actions in Input Manager
    ImGui::Text("Buttons"); ImGui::Separator();
    for (auto& button : userContext->m_single)
    {
        std::string treeLabel = "##button" + std::to_string(i);

        if (ImGui::Button((std::string("-") + treeLabel).c_str()))
        {
            IM->PushContext(userContext);
            int success = IM->RemoveActionSingle(button.first);
            IM->PopContext();

            if (success == 1)
            {
                std::string notif = "New input added to Action.";
                EditorManager::SendNotification(notif.c_str(), ENotifType::Success, 1.0f);
                break;
            }
            else if (success == -1)
            {
                std::string notif = "Couldn't add input to Action.";
                EditorManager::SendNotification(notif.c_str(), ENotifType::Error, 1.0f);
            }
        } ImGui::SameLine();

        if (ImGui::TreeNode((button.first + treeLabel).c_str()))
        {
            // Loop in keys in current action
            for (auto& input : button.second.Inputs())
            {
                bool breakLoop = false;

                if (m_inputsIndex.size() <= j)
                    m_inputsIndex.push_back(IM->GetKeycodeIndex(input.first));
                else
                    m_inputsIndex[j] = IM->GetKeycodeIndex(input.first);

                const char* combo_preview_value = IM->GetKeyname(m_inputsIndex[j]);

                ImGui::Text("Key:"); ImGui::SameLine();
                std::string label = "##k" + std::to_string(j);

                // Key selection combo
                if (ImGui::BeginCombo(label.c_str(), combo_preview_value))
                {
                    for (int n = 0; n < IM->KeynamesSize(); n++)
                    {
                        const bool is_selected = (m_inputsIndex[j] == n);
                        if (ImGui::Selectable(IM->GetKeyname(n), is_selected))
                        {
                            m_inputsIndex[j] = n;
                            int success = IM->ChangeInputInAction(&button.second, input.first, IM->GetKeycode(m_inputsIndex[j]));
                            breakLoop = true;

                            if (success == 0)
                            {
                                std::string notif = "Key " + std::string(IM->GetKeyname(m_inputsIndex[j])) + " is already link to Action " + button.first;
                                EditorManager::SendNotification(notif.c_str(), ENotifType::Warning, 1.0f);
                            }
                        }

                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();

                } ImGui::SameLine();

                // Start listening for key
                std::string listeningLabel = "Listening" + label;
                if (ImGui::Button((std::string("...") + label).c_str()))
                {
                    IM->SetListening();

                    ImGui::OpenPopup(listeningLabel.c_str());
                }

                // Listening
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                if (ImGui::BeginPopupModal(listeningLabel.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Separator();

                    int key = IM->GetListenedKey();
                    if (key != -1)
                    {
                        IM->ChangeInputInAction(&button.second, input.first, IM->GetKeycode(key));
                        breakLoop = true;

                        IM->ResetListenedKey();

                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                } ImGui::SameLine();

                // Remove key from Action
                if (ImGui::Button((std::string("-") + label).c_str()))
                {
                    IM->PushContext(userContext);
                    int success = IM->RemoveInputFromAction(&button.second, input.first);
                    IM->PopContext();

                    if (success == 1)
                    {
                        std::string notif = "New input added to Action.";
                        EditorManager::SendNotification(notif.c_str(), ENotifType::Success, 1.0f);
                        breakLoop = true;
                    }
                    else if (success == -1)
                    {
                        std::string notif = "Couldn't add input to Action.";
                        EditorManager::SendNotification(notif.c_str(), ENotifType::Error, 1.0f);
                    }
                }

                ImGui::Separator();

                j++;

                if (breakLoop)
                    break;
            }

            // Add new key to Action
            if (ImGui::Button("+"))
            {
                IM->PushContext(userContext);
                int success = IM->AddInputToAction(&button.second, Keycode::UNKNOWN);
                IM->PopContext();

                if (success == 1)
                {
                    std::string notif = "New input added to Action.";
                    EditorManager::SendNotification(notif.c_str(), ENotifType::Success, 1.0f);
                }
                else if (success == -1)
                {
                    std::string notif = "Couldn't add input to Action.";
                    EditorManager::SendNotification(notif.c_str(), ENotifType::Error, 1.0f);
                }
            }
            ImGui::TreePop();
        }
        i++;
    }
}

void ProjectSettingsDisplayer::Input::SetAxes(InputManager* IM)
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    int i = 0, j = 0;

    // Loop over Axes actions in Input Manager
    ImGui::Text("Axes"); ImGui::Separator();
    for (auto& axis : userContext->m_axes)
    {
        std::string treeLabel = "##axis" + std::to_string(i);

        if (ImGui::Button((std::string("-") + treeLabel).c_str()))
        {
            IM->PushContext(userContext);
            int success = IM->RemoveActionAxes(axis.first);
            IM->PopContext();

            if (success == 1)
            {
                std::string notif = "New input added to Action.";
                EditorManager::SendNotification(notif.c_str(), ENotifType::Success, 1.0f);
                break;
            }
            else if (success == -1)
            {
                std::string notif = "Couldn't add input to Action.";
                EditorManager::SendNotification(notif.c_str(), ENotifType::Error, 1.0f);
            }
        } ImGui::SameLine();

        if (ImGui::TreeNode((axis.first + treeLabel).c_str()))
        {
            // Loop over axis in current action
            for (auto& axes : axis.second.Axes())
            {
                const char* combo_preview_value = nullptr;
                std::string label = "";
                int* currentKey = nullptr;

                // Loop over positive and negative key of the axis
                for (int k = 0; k < 2; k++)
                {
                    if (k == 0)
                    {
                        if (m_axisPosIndex.size() <= j)
                            m_axisPosIndex.push_back(IM->GetKeycodeIndex(axes->GetPositiveKey()));
                        else
                            m_axisPosIndex[j] = IM->GetKeycodeIndex(axes->GetPositiveKey());

                        currentKey = &m_axisPosIndex[j];

                        ImGui::Text("Positive:");
                        ImGui::SameLine();
                        label = "##pos" + std::to_string(j);
                    }
                    else if (k == 1)
                    {
                        if (m_axisNegIndex.size() <= j)
                            m_axisNegIndex.push_back(IM->GetKeycodeIndex(axes->GetNegativeKey()));
                        else
                            m_axisNegIndex[j] = IM->GetKeycodeIndex(axes->GetNegativeKey());

                        currentKey = &m_axisNegIndex[j];

                        ImGui::Text("Negative:");
                        ImGui::SameLine();
                        label = "##neg" + std::to_string(j);
                    }

                    if (currentKey)
                    {
                        combo_preview_value = IM->GetKeyname(*currentKey);

                        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f - 150.f);
                        // Key selection combo
                        if (ImGui::BeginCombo(label.c_str(), combo_preview_value))
                        {
                            for (int n = 0; n < IM->KeynamesSize(); n++)
                            {
                                const bool is_selected = (*currentKey == n);

                                if (ImGui::Selectable(IM->GetKeyname(n), is_selected))
                                {
                                    *currentKey = n;
                                    
                                    IM->SetKey(axes, IM->GetKeycode(*currentKey), (bool)k);
                                }

                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        } ImGui::SameLine();
                    }

                    // Start listening for key
                    std::string listeningLabel = "Listening" + label;
                    if (ImGui::Button((std::string("...") + label).c_str()))
                    {
                        IM->SetListening();

                        ImGui::OpenPopup(listeningLabel.c_str());
                    }

                    // Listening
                    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                    if (ImGui::BeginPopupModal(listeningLabel.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
                    {
                        ImGui::Separator();

                        int key = IM->GetListenedKey();
                        if (key != -1)
                        {
                            IM->SetKey(axes, IM->GetKeycode(key), (bool)k);
                            IM->ResetListenedKey();

                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    } ImGui::SameLine();
                }

                // Add new key to Action
                if (ImGui::Button((std::string("-") + label).c_str()))
                {
                    IM->PushContext(userContext);
                    int success = IM->RemoveAxisFromAction(&axis.second, axes);
                    IM->PopContext();

                    if (success == 1)
                    {
                        std::string notif = "New input added to Action.";
                        EditorManager::SendNotification(notif.c_str(), ENotifType::Success, 1.0f);
                    }
                    else if (success == -1)
                    {
                        std::string notif = "Couldn't add input to Action.";
                        EditorManager::SendNotification(notif.c_str(), ENotifType::Error, 1.0f);
                    }
                }

                j++;
            }

            // Add new axis to Action
            if (ImGui::Button("+"))
            {
                IM->PushContext(userContext);
                int success = IM->AddAxisToAction(&axis.second, { Keycode::UNKNOWN, Keycode::UNKNOWN });
                IM->PopContext();

                if (success == 1)
                {
                    std::string notif = "New axis added to Action.";
                    EditorManager::SendNotification(notif.c_str(), ENotifType::Success, 1.0f);
                }
                else if (success == -1)
                {
                    std::string notif = "Couldn't add axis to Action.";
                    EditorManager::SendNotification(notif.c_str(), ENotifType::Error, 1.0f);
                }
            }
            ImGui::TreePop();
        }
        i++;
    }
}

void ProjectSettingsDisplayer::Audio::Fill()
{
    IMGUI_LEFT_LABEL(ImGui::Checkbox, "3D Spatialization :", &m_isUsingSpatialization);
    IMGUI_LEFT_LABEL(ImGui::SliderFloat, "Global Volume :", &m_globalVolume, 0.f, 100.f);
}

void ProjectSettingsDisplayer::Physics::Fill()
{
    static char gravity[32] = "9.81";
    IMGUI_LEFT_LABEL(ImGui::InputText, "Gravity :", gravity, IM_ARRAYSIZE(gravity));

    m_gravity = std::stof(gravity);


    ImGui::Separator();
    ImGui::Text("Layer Matrix");
}

void ProjectSettingsDisplayer::TagLayer::Fill()
{
    ImGui::Text("Tag List");
    ImGui::Separator();
    ImGui::Text("Layers List");
}

void ProjectSettingsDisplayer::RenderPass::Fill()
{
    ImGui::Text("Pass Order");
}

void ProjectSettingsDisplayer::ResourceViewer::Fill()
{
    /*ImGui::Text("Resource count %i", ResourceManager::GetInstance()->GetResourceCount());

    std::map<std::type_index, std::vector<const char*>> resourcePaths;
    std::vector<const char*> resourcesType;
    std::vector<std::type_index> resourceTypeId;
    ResourceManager::GetInstance()->GetResourcesPath(resourcePaths);

    for (auto& pair : resourcePaths)
    {
        resourcesType.push_back(pair.first.name());
        resourceTypeId.push_back(pair.first);
    }

    static const char* current_item = NULL;
    static size_t current_id = 0;

    if (ImGui::BeginCombo("##combo", current_item))
    {
        for (size_t n = 0; n < resourcesType.size(); n++)
        {
            bool is_selected = (current_item == resourcesType[n]);
            if (ImGui::Selectable(resourcesType[n], is_selected))
            {
                current_item = resourcesType[n];
                current_id = n;
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }
        ImGui::EndCombo();
    }

    auto it = resourcePaths.find(resourceTypeId[current_id]);
    if (it != resourcePaths.end())
    {
        for (int i = 0; i < it->second.size(); i++)
        {
            ImGui::Text(it->second[i]);
        }
    }   */
}

BuildSettings ProjectSettingsDisplayer::GetBuildSettings()
{
    BuildSettings settings;
    ProjectSettingsDisplayer::General* g = (ProjectSettingsDisplayer::General*)m_categories[0];
    settings.m_gameName = g->name;

    return settings;
}
