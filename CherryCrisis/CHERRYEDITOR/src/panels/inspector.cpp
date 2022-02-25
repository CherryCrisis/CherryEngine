
#include "panels/inspector.hpp"

#include <imgui.h>
#include <string>
#include <typeinfo>

#include "cherry_header.hpp"

void Inspector::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Inspector", &m_isOpened))
    {
        for (const auto comp : m_engine->behaviours)
        {
           std::string name = typeid(*comp).name();
           name = name.substr(name.find_first_of(" \t") + 1);

            ImGui::Text(name.c_str());
        }
    }
    ImGui::End();
}