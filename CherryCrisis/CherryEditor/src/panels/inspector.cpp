
#include "panels/inspector.hpp"

#include <imgui.h>
#include <string>
#include <typeinfo>

#include "cherry_header.hpp"
#include "core/editor_manager.hpp"

#include "transform.hpp"
#include "light_component.hpp"
#include "camera_component.hpp"
#include "scripted_behaviour.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))


Inspector::Inspector(bool spawnOpened, EditorManager* manager) : Panel(spawnOpened), m_manager(manager)
{
    
}

void InspectComponents(Entity* entity, int id)
{
        Transform* transform;

        if (!entity->TryGetBehaviour<Transform>(transform) || !transform->IsRoot())
            return;

        if (ImGui::TreeNode((void*)(intptr_t)id, "Instance %i", id))
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
                Metadata& metadatas = scriptedBehaviour->m_metadatas;
                std::vector<Field>& fields = metadatas.m_fields;

                for (int i = 0; i < metadatas.m_fields.size(); i++)
                {
                    Field& field = fields[i];

                    switch (field.m_type)
                    {
                    case DescriptorType::VECTOR3:
                        if (field.m_isRef)
                        {
                            CCMaths::Vector3* vec = (CCMaths::Vector3*)field.m_ptr;
                            ImGui::DragFloat3(field.m_name.c_str(), vec->data, 0.5f);
                        }
                    }
                }
            }

            ImGui::TreePop();
        }
}

void Inspector::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Inspector", &m_isOpened))
    {
        int i = 0;
        for (auto& entity : m_manager->m_selectedEntities)
        {
            InspectComponents(entity, i);
            i++;
        }
    }
    ImGui::End();
}


void InspectVector2(void* vector) 
{

}

void InspectVector3(void* vector)
{

}