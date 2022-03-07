
#include "panels/project_settings_displayer.hpp"

#include <imgui.h>

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

ProjectSettingsDisplayer::ProjectSettingsDisplayer(bool spawnOpened = true) : Panel(spawnOpened) 
{
    m_categories[0] = new General( "Generals" );
    m_categories[1] = new Audio("Audio");
    m_categories[2] = new Physics("Physics");
    m_categories[3] = new Input("Inputs");
    m_categories[4] = new TagLayer("Tags / Layers");
    m_categories[5] = new RenderPass("Render Pass");
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
            for (PanelCategory* category : m_categories )
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

void ProjectSettingsDisplayer::Input::Fill()
{
    ImGui::Text("Inputs ");
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