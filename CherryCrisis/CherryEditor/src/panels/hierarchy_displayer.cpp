
#include "panels/hierarchy_displayer.hpp"

#include <imgui.h>

#include "scene.hpp"
#include "transform.hpp"
#include "light_component.hpp"
#include "camera_component.hpp"
#include "scripted_behaviour.hpp"

void HierarchyDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Hierarchy", &m_isOpened))
    {
        int count = 0;
        for (auto& [entityName, entity] : m_displayedScene->m_entities)
        {
            Transform* transform;
            
            if (!entity->TryGetBehaviour<Transform>(transform) || !transform->IsRoot())
                continue;

            if (ImGui::TreeNode((void*)(intptr_t)count, "Instance %i", count))
            {
                CameraComponent* cameraComponent;
                if (entity->TryGetBehaviour<CameraComponent>(cameraComponent))
                {
                    Camera& cam = cameraComponent->m_camera;
                    ImGui::DragFloat("Near", &cam.near);
                    ImGui::DragFloat("Far", &cam.far);
                    ImGui::DragFloat("Fov", &cam.fovY);
                }

                LightComponent* lightComponent;
                if (entity->TryGetBehaviour<LightComponent>(lightComponent))
                {
                    Light& light = lightComponent->m_light;
                    ImGui::Checkbox("LightIsPoint", &light.m_isPoint);
                    ImGui::DragFloat3("LightPosition", light.m_position.data, 0.5f);
                    ImGui::DragFloat3("LightAmbient", light.m_ambient.data, 0.5f);
                    ImGui::DragFloat3("LightDiffuse", light.m_diffuse.data, 0.5f);
                    ImGui::DragFloat3("LightSpecular", light.m_specular.data, 0.5f);
                }

                if (transform)
                {
                    Vector3 position = transform->GetPosition();

                    if (ImGui::DragFloat3("Position", position.data, 0.5f))
                        transform->SetPosition(position);

                    Vector3 rotation = transform->GetRotation() * CCMaths::RAD2DEG;

                    if (ImGui::DragFloat3("Rotation", rotation.data, 0.5f))
                        transform->SetRotation(rotation * CCMaths::DEG2RAD);

                    Vector3 scale = transform->GetScale();

                    if (ImGui::DragFloat3("Scale", scale.data, 0.5f))
                        transform->SetScale(scale);
                }


                ScriptedBehaviour* scriptedBehaviour;
                if (entity->TryGetBehaviour<ScriptedBehaviour>(scriptedBehaviour))
                {
                    std::vector<Field>& fields = scriptedBehaviour->GetFields();

                    for (int i = 0; i < fields.size(); i++)
                    {
                        Field& field = fields[i];

                        switch (field.m_type)
                        {
                        case DescriptorType::VECTOR3:
                            if (field.m_isRef)
                            {
                                CCMaths::Vector3* vec = std::any_cast<CCMaths::Vector3*>(field.value);
                                ImGui::DragFloat3(field.m_name.c_str(), vec->data, 0.5f);
                            }
                        case DescriptorType::INT:
                            if (!field.m_isRef)
                            {
                                int num = std::any_cast<int>(field.value);
                                ImGui::DragInt(field.m_name.c_str(), &num, 0.5f);
                            }
                        }
                    }
                }

                ImGui::TreePop();
            }

            count++;
        }
    }
    ImGui::End();
}
