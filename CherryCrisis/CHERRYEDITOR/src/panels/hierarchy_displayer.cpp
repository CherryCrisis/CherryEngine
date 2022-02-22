
#include "panels/hierarchy_displayer.hpp"

#include <imgui.h>

#include "scene.hpp"
#include "transform.hpp"

void HierarchyDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Hierarchy", &m_isOpened))
    {
        for (size_t i = 0; i < m_displayedScene->m_entities.size(); i++)
        {
            Entity& entity = m_displayedScene->m_entities[i];

            if (!entity.m_transform->IsRoot())
                continue;

            if (ImGui::TreeNode((void*)(intptr_t)i, "Instance %i", i))
            {
                if (entity.m_light)
                {
                    Light& light = entity.m_light->m_light;
                    ImGui::DragFloat3("LightPosition", light.m_position.data, 0.5f);
                    ImGui::DragFloat3("LightAmbient", light.m_ambient.data, 0.5f);
                    ImGui::DragFloat3("LightDiffuse", light.m_diffuse.data, 0.5f);
                    ImGui::DragFloat3("LightSpecular", light.m_specular.data, 0.5f);

                }

                Vector3 position = entity.m_transform->GetPosition();

                if (ImGui::DragFloat3("Position", position.data, 0.5f))
                    entity.m_transform->SetPosition(position);

                Vector3 rotation = entity.m_transform->GetRotation() * CCMaths::RAD2DEG;

                if (ImGui::DragFloat3("Rotation", rotation.data, 0.5f))
                    entity.m_transform->SetRotation(rotation * CCMaths::DEG2RAD);

                Vector3 scale = entity.m_transform->GetScale();

                if (ImGui::DragFloat3("Scale", scale.data, 0.5f))
                    entity.m_transform->SetScale(scale);


                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}
