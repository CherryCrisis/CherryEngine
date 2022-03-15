
#include "panels/inspector.hpp"

#include <imgui.h>
#include <string>
#include <typeinfo>

#include "cherry_header.hpp"
#include "core/editor_manager.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))


Inspector::Inspector(bool spawnOpened, EditorManager* manager) : Panel(spawnOpened), m_manager(manager)
{
    
}

void InspectComponents(Entity entity, int id)
{
    std::string name = typeid(entity).name();
    name = name.substr(name.find_first_of(" \t") + 1);

    if (entity.m_cameraComp)
    {
        Camera& cam = entity.m_cameraComp->m_camera;
        ImGui::DragFloat("Near", &cam.near);
        ImGui::DragFloat("Far", &cam.far);
        ImGui::DragFloat("Fov", &cam.fovY);
        ImGui::Spacing();
        ImGui::Separator();
    }
    if (entity.m_lightComp)
    {
        Light& light = entity.m_lightComp->m_light;
        ImGui::Checkbox("LightIsPoint", &light.m_isPoint);
        ImGui::DragFloat3("LightPosition", light.m_position.data, 0.5f);
        ImGui::DragFloat3("LightAmbient", light.m_ambient.data, 0.5f);
        ImGui::DragFloat3("LightDiffuse", light.m_diffuse.data, 0.5f);
        ImGui::DragFloat3("LightSpecular", light.m_specular.data, 0.5f);
        ImGui::Spacing();
        ImGui::Separator();
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
}

void Inspector::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Inspector", &m_isOpened))
    {
        int i = 0;
        for (const auto comp : m_manager->m_selectedEntities)
        {
            InspectComponents(*comp,i);
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