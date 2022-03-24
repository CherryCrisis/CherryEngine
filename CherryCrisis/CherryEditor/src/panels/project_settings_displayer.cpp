
#include "panels/project_settings_displayer.hpp"

#include <imgui.h>

#include "core/editor_manager.hpp"
#include "resource_manager.hpp"

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

        ImGui::End();
    }
}


void ProjectSettingsDisplayer::General::Fill()
{
    static char name[32] = "Cherry Template";
    IMGUI_LEFT_LABEL(ImGui::InputText, "Game Name :", name, IM_ARRAYSIZE(name));

    static char version[32] = "0.0.1";
    IMGUI_LEFT_LABEL(ImGui::InputText, "Version :", version, IM_ARRAYSIZE(version));

    static char company[32] = "Cherry";
    IMGUI_LEFT_LABEL(ImGui::InputText, "Company :", company, IM_ARRAYSIZE(company));
}

ProjectSettingsDisplayer::Input::Input(std::string name) : PanelCategory(name)
{
    userContext = InputManager::GetInstance()->AddContext("user Context");
}

void ProjectSettingsDisplayer::Input::Fill()
{
    InputManager* IM = InputManager::GetInstance();

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    if (ImGui::Button("Add Axis Mapped Action"))
    {
        ImGui::OpenPopup("Action Axes Creation");
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Button Mapped Action"))
    {
        ImGui::OpenPopup("Action Buttons Creation");
    }

#pragma region degeu 
    //TODO ---- TO MOVE INTO A PROPER FUNCTION
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Action Axes Creation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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

            int success = 0;
            IM->SetContext(userContext);
            IM->AddActionAxes(std::string(name), success);
            IM->SetContext(nullptr);
            
            if (success == 1)
            {
                std::string notif = "Action Axes " + std::string(name) + " created.";
                EditorManager::SendNotification(notif.c_str(), ImGuiToastType::Success, 1.0f);
            }
            else if (success == 0)
            {
                std::string notif = "Action Axes " + std::string(name) + " already exists.";
                EditorManager::SendNotification(notif.c_str(), ImGuiToastType::Warning, 1.0f);
            }
            else if (success == -1)
            {
                std::string notif = "Action Axes " + std::string(name) + " couldn't be created.";
                EditorManager::SendNotification(notif.c_str(), ImGuiToastType::Error, 1.0f);
            }

            memset(name, 0, IM_ARRAYSIZE(name));
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Action Buttons Creation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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

            int success = 0;
            IM->SetContext(userContext);
            IM->AddActionButtons(std::string(name), success);
            IM->SetContext(nullptr);

            if (success == 1)
            {
                std::string notif = "Action Buttons " + std::string(name) + " created.";
                EditorManager::SendNotification(notif.c_str(), ImGuiToastType::Success, 1.0f);
            }
            else if (success == 0)
            {
                std::string notif = "Action Buttons " + std::string(name) + " already exists.";
                EditorManager::SendNotification(notif.c_str(), ImGuiToastType::Warning, 1.0f);
            }
            else if (success == -1)
            {
                std::string notif = "Action Buttons " + std::string(name) + " couldn't be created.";
                EditorManager::SendNotification(notif.c_str(), ImGuiToastType::Error, 1.0f);
            }

            memset(name, 0, IM_ARRAYSIZE(name));
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
#pragma endregion
    int i = 0;
    int j = 0;
    ImGui::Text("Axes"); ImGui::Separator();

    for (auto& axis : userContext->m_axes)
    {
        std::string treeLabel = axis.first + "##axis" + std::to_string(i);
        if (ImGui::TreeNode(treeLabel.c_str()))
        {
            for (auto& axes : axis.second.Axes())
            {
                if (m_axisPosIndex.size() <= j)
                    m_axisPosIndex.push_back(IM->GetKeycodeIndex(axes->GetPositiveKey()));
                else
                    m_axisPosIndex[j] = IM->GetKeycodeIndex(axes->GetPositiveKey());

                if (m_axisNegIndex.size() <= j)
                    m_axisNegIndex.push_back(IM->GetKeycodeIndex(axes->GetNegativeKey()));
                else
                    m_axisNegIndex[j] = IM->GetKeycodeIndex(axes->GetNegativeKey());

                const char* combo_preview_valueP = IM->GetKeyname(m_axisPosIndex[j]);
                const char* combo_preview_valueN = IM->GetKeyname(m_axisNegIndex[j]);

                ImGui::Text("Positive key:"); ImGui::SameLine();
                std::string label = "##pos" + std::to_string(j);

                if (ImGui::BeginCombo(label.c_str(), combo_preview_valueP))
                {
                    for (int n = 0; n < IM->KeynamesSize(); n++)
                    {
                        const bool is_selected = (m_axisPosIndex[j] == n);
                        if (ImGui::Selectable(IM->GetKeyname(n), is_selected))
                        {
                            m_axisPosIndex[j] = n;
                            IM->SetPositiveKey(axes, IM->GetKeycode(m_axisPosIndex[j]));
                        }
                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                ImGui::Text("Negative key:"); ImGui::SameLine();
                label = "##neg" + std::to_string(j);

                if (ImGui::BeginCombo(label.c_str(), combo_preview_valueN))
                {
                    for (int n = 0; n < IM->KeynamesSize(); n++)
                    {
                        const bool is_selected = (m_axisNegIndex[j] == n);
                        if (ImGui::Selectable(IM->GetKeyname(n), is_selected))
                        {
                            m_axisNegIndex[j] = n;
                            IM->SetNegativeKey(axes, IM->GetKeycode(m_axisNegIndex[j]));
                        }
                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::Separator();

                j++;
            }

            if (ImGui::Button("+"))
            {
                IM->SetContext(userContext);
                int success = IM->AddAxisToAction(&axis.second, { Keycode::UNKNOWN, Keycode::UNKNOWN });
                IM->SetContext(nullptr);

                if (success == 1)
                {
                    std::string notif = "New axis added to Action.";
                    EditorManager::SendNotification(notif.c_str(), ImGuiToastType::Success, 1.0f);
                }
                else if (success == -1)
                {
                    std::string notif = "Couldn't add axis to Action.";
                    EditorManager::SendNotification(notif.c_str(), ImGuiToastType::Error, 1.0f);
                }
            }
            ImGui::TreePop();
        }
        i++;
    }

    i = 0;
    j = 0;
    ImGui::Spacing(); ImGui::Separator();
    ImGui::Text("Buttons"); ImGui::Separator();
    for (auto& button : userContext->m_buttons)
    {
        std::string treeLabel = button.first + "##button" + std::to_string(i);
        if (ImGui::TreeNode(treeLabel.c_str()))
        {
            for (auto& input : button.second.Inputs())
            {
                bool breakLoop = false;

                if (m_inputsIndex.size() <= j)
                    m_inputsIndex.push_back(IM->GetKeycodeIndex(input.first));
                else
                    m_inputsIndex[j] = IM->GetKeycodeIndex(input.first);

                const char* combo_preview_valueK = IM->GetKeyname(m_inputsIndex[j]);

                ImGui::Text("Key:"); ImGui::SameLine();
                std::string label = "##k" + std::to_string(j);

                if (ImGui::BeginCombo(label.c_str(), combo_preview_valueK))
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
                                EditorManager::SendNotification(notif.c_str(), ImGuiToastType::Warning, 1.0f);
                            }
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();

                    if (breakLoop)
                        break;
                }
                ImGui::Separator();

                j++;
            }

            if (ImGui::Button("+"))
            {
                IM->SetContext(userContext);
                int success = IM->AddInputToAction(&button.second, Keycode::UNKNOWN);
                IM->SetContext(nullptr);

                if (success == 1)
                {
                    std::string notif = "New input added to Action.";
                    EditorManager::SendNotification(notif.c_str(), ImGuiToastType::Success, 1.0f);
                }
                else if (success == -1)
                {
                    std::string notif = "Couldn't add input to Action.";
                    EditorManager::SendNotification(notif.c_str(), ImGuiToastType::Error, 1.0f);
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
    ImGui::Text("Resource count %i", ResourceManager::GetInstance()->GetResourceCount());

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
    }
    
}