
#include "panels/preferences_displayer.hpp"

#include <imgui.h>

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

PreferencesDisplayer::PreferencesDisplayer(bool spawnOpened) : Panel(spawnOpened) 
{
    m_categories[0] = new Cutstomization("Cutstomization");
    m_categories[1] = new ThirdParty("Third Party");
}

void PreferencesDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(.5f, .5f));
    if (ImGui::Begin("Preferences", &m_isOpened, ImGuiWindowFlags_MenuBar))
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

void PreferencesDisplayer::Cutstomization::Fill()
{
    ImGui::Text("Language");
    ImGui::Separator();
    ImGui::Text("Color Editor");
    ImGui::Text("Shortcut Editor");
}

void PreferencesDisplayer::ThirdParty::Fill()
{
    static char scripting[32] = "Scripting App Path";
    IMGUI_LEFT_LABEL(ImGui::InputText, "Scripting Software:", scripting, IM_ARRAYSIZE(scripting));

    static char shader[32] = "Shader App Path";
    IMGUI_LEFT_LABEL(ImGui::InputText, "Shader Editing Software:", shader, IM_ARRAYSIZE(shader));
}