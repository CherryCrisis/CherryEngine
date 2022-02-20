
#include "panels/hierarchy_displayer.hpp"

#include <imgui.h>

#include "scene.hpp"

void HierarchyDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Hierarchy", &m_isOpened))
    {

        for (size_t i = 0; i < m_displayedScene->m_entities.size(); i++)
        {
            Entity& entity = m_displayedScene->m_entities[i];

            if (ImGui::TreeNode((void*)(intptr_t)i, "Instance %i", i))
            {
                Vector3 position = entity.m_transform->GetPosition();

                if (ImGui::DragFloat3("Position", position.data))
                    entity.m_transform->SetPosition(position);

                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}
