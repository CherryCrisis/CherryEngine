
#include "panels/inspector.hpp"

#include <imgui.h>
#include <string>
#include <typeinfo>
#include <algorithm>

#include "cherry_header.hpp"
#include "core/editor_manager.hpp"

#include "transform.hpp"
#include "light_component.hpp"
#include "camera_component.hpp"
#include "portal_component.hpp"
#include "scripted_behaviour.hpp"
#include "csscripting_system.hpp"
#include "rigidbody.hpp"
#include "box_collider.hpp"
#include "sphere_collider.hpp"
#include "capsule_collider.hpp"
#include "bool3.hpp"
#include "core/imcherry.hpp"
#include "utils.hpp"
#include "basic_renderpass.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))


Inspector::Inspector(bool spawnOpened, EditorManager* manager) : Panel(spawnOpened), m_manager(manager)
{
    
}

std::string ExtractValue(std::string str, const char key = ':')
{
    return str.substr(str.find_first_of(key) + 1);
}

void InspectComponents(Entity* entity, int id)
{
    std::vector<Behaviour*> behaviours = entity->GetAllBehaviours();

    ModelRenderer* renderer = nullptr;
    for (Behaviour* behaviour : behaviours)
    {
        ImGui::PushID(static_cast<int>(behaviour->GetUUID()));
        std::string bname = typeid(*behaviour).name();
        bname = ExtractValue(bname, ' ');

        if (ImGui::BeginPopupContextItem("context"))
        {
            if (ImGui::MenuItem("Delete")) 
            {
                if (entity->RemoveBehaviour(behaviour)) 
                {
                    ImGui::EndPopup(); ImGui::PopID();
                    break;
                }

            }
            if (ImGui::MenuItem("Copy")) {}
            if (ImGui::MenuItem("Paste")) {}
            ImGui::EndPopup();
        }
        bool opened = false;
        ScriptedBehaviour* b = (ScriptedBehaviour*)behaviour;
        if (bname == "ScriptedBehaviour")
            opened = ImGui::TreeNode(b->GetScriptPath().c_str());
        else
            opened = ImGui::TreeNode(bname.c_str());
        if (bname == "ModelRenderer")
            renderer = (ModelRenderer*)behaviour;
        // check if right clicked
        if (InputManager::GetInstance()->GetKeyDown(Keycode::RIGHT_CLICK) && ImGui::IsItemHovered())
        {
            ImGui::OpenPopup("context");
        }

        ImVec4 color1 = { 100.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f };
        ImVec4 color2 = { 18.f / 255.f, 120.f / 255.f, 4.f / 255.f, 1.f };
        ImVec4 color3 = { 12.f / 255.f, 50.f / 255.f, 170.f / 255.f, 1.f };

        if (opened)
        {
            auto& fields = behaviour->GetFields();
            for (auto& [fieldName, fieldRef] : fields)
            {
                const std::type_index type = fieldRef.m_value.type();

                if (type == typeid(CCMaths::Vector3))
                {
                    CCMaths::Vector3 val = std::any_cast<CCMaths::Vector3>(fieldRef.m_value);
                    ImCherry::ColoredDragFloat3(fieldRef.m_name.c_str(), val.data, {1.f,1.f,1.f,1.f}, 0.5f);
                    continue;
                }

                if (type == typeid(CCMaths::Vector3*))
                {
                    CCMaths::Vector3* val = std::any_cast<CCMaths::Vector3*>(fieldRef.m_value);
                    ImCherry::ColoredDragFloat3(fieldRef.m_name.c_str(), val->data, color1, color2, color3, 0.5f);
                    continue;
                }

                if (type == typeid(CCMaths::Vector3**))
                {
                    CCMaths::Vector3** val = std::any_cast<CCMaths::Vector3**>(fieldRef.m_value);
                    CCMaths::Vector3* v = *val;
                    ImCherry::ColoredDragFloat3(fieldRef.m_name.c_str(), v->data, color1, color2, color3 , 0.5f);
                    continue;
                }

                if (type == typeid(Bool3))
                {
                    Bool3 val = std::any_cast<Bool3>(fieldRef.m_value);
                    ImGui::Checkbox(fieldRef.m_name.c_str(), &val.x); ImGui::SameLine();
                    ImGui::Checkbox(fieldRef.m_name.c_str(), &val.y); ImGui::SameLine();
                    ImGui::Checkbox(fieldRef.m_name.c_str(), &val.z);
                    continue;
                }
                if (type == typeid(bool))
                {
                    bool val = std::any_cast<bool>(fieldRef.m_value);
                    ImGui::Checkbox(fieldRef.m_name.c_str(), &val);
                    continue;
                }
                if (type == typeid(bool*))
                {
                    bool* val = std::any_cast<bool*>(fieldRef.m_value);
                    ImGui::Checkbox(fieldRef.m_name.c_str(), val);
                    continue;
                }
                if (type == typeid(int*))
                {
                    int* val = std::any_cast<int*>(fieldRef.m_value);
                    ImGui::DragInt(fieldRef.m_name.c_str(), val, 0.5f);
                    continue;
                }
                if (type == typeid(float*))
                {
                    float* val = std::any_cast<float*>(fieldRef.m_value);
                    ImGui::DragFloat(fieldRef.m_name.c_str(), val, 0.5f, 0.0f, 0.0f, *val >= 1.0e+5 ? "%e" : "%.3f");
                    continue;
                }
                if (type == typeid(float))
                {
                    float val = std::any_cast<float>(fieldRef.m_value);
                    ImGui::DragFloat(fieldRef.m_name.c_str(), &val, 0.5f, 0.0f, 0.0f, val >= 1.0e+5 ? "%e" : "%.3f");
                    continue;
                }

                if (type == typeid(std::string*))
                {
                    std::string val = *std::any_cast<std::string*>(fieldRef.m_value);
                    ImGui::InputText(fieldRef.m_name.c_str(), &val[0], val.size() + 1);
                    continue;
                }
            }

            std::unordered_map <std::string, CCProperty::IClearProperty*>& properties = behaviour->GetProperties();

            for (const auto& [propName, propRef] : properties)
            {
                auto& propType = propRef->GetGetType();

                if (propType == typeid(CCMaths::Vector3))
                {
                    CCMaths::Vector3 val;
                    propRef->Get(&val); 
                    if (ImCherry::ColoredDragFloat3(propName.c_str(), val.data, color1, color2, color3, 0.5f))
                        propRef->Set(&val);

                    continue;
                }

                if (propType == typeid(int))
                {
                    int val;
                    propRef->Get(&val);
                    if (ImGui::DragInt(propName.c_str(), &val, 0.5f))
                        propRef->Set(&val);

                    continue;
                }

                if (propType == typeid(bool))
                {
                    bool val;
                    propRef->Get(&val);
                    if (ImGui::Checkbox(propName.c_str(), &val))
                        propRef->Set(&val);

                    continue;
                }

                if (propType == typeid(Bool3))
                {
                    Bool3 val;
                    propRef->Get(&val);
                    if (ImGui::TreeNode(propName.c_str()))
                    {
                        ImGui::Checkbox("X", &val.x); ImGui::SameLine();
                        ImGui::Checkbox("Y", &val.y); ImGui::SameLine();
                        ImGui::Checkbox("Z", &val.z);
                        propRef->Set(&val);

                        ImGui::TreePop();
                    }
                    continue;
                }

                if (propType == typeid(float))
                {
                    float val;
                    propRef->Get(&val);
                    if (ImGui::DragFloat(propName.c_str(), &val, 0.5f, 0.0f, 0.0f, val >= 1.0e+5 ? "%e" : "%.3f"))
                        propRef->Set(&val);


                    continue;
                }

                if (propType == typeid(std::string))
                {
                    std::string val;
                    propRef->Get(&val);
                    if (ImGui::InputText(propName.c_str(), &val[0], val.size() + 2))
                        propRef->Set(&val);

                    continue;
                }

                if (propType == typeid(char*))
                {
                    char* val;
                    propRef->Get(&val);
                    if (ImGui::InputText(propName.c_str(), val, strlen(val) + 2))
                        propRef->Set(&val);

                    continue;
                }

                if (propType == typeid(const char*))
                {
                    const char* val = nullptr;
                    propRef->Get(&val);

                    ImGui::Text("%s %s", propName.c_str(), val ? val : "X");

                    continue;
                }
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        ImGui::PopID();
    }
    
    //Inspect Material
    if (renderer && renderer->m_model && renderer->m_model->m_material)
    {
        std::shared_ptr<Material> mat = renderer->m_model->m_material;
        if (ImGui::TreeNode("Material"))
        {
            
            const char* label = "Material Pipeline";
            const char* list[] = { "LIT", "PBR"};

            unsigned int currentId = (unsigned int)mat->m_pipelineType;

            if (ImGui::BeginCombo(label, list[currentId]))
            {
                size_t listSize = std::size(list);
                for (size_t n = 0; n < listSize; n++)
                {
                    const bool is_selected = (currentId == n);
                    if (ImGui::Selectable(list[n], is_selected))
                    {
                        mat->m_pipelineType = (EPipelineType)n;
                    }

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (EPipelineType::LIT == mat->m_pipelineType)
            {
                ImGui::Text(mat->GetFilepath().c_str());
                ImGui::DragFloat3("Ambient", mat->m_ambient.data, 0.1f);
                ImGui::DragFloat3("Diffuse", mat->m_diffuse.data, 0.1f);
                ImGui::DragFloat3("Specular", mat->m_specular.data, 0.1f);
                ImGui::DragFloat3("Emissive", mat->m_emissive.data, 0.1f);
                ImGui::DragFloat("Shininess", &mat->m_shininess, 0.1f);
            }
            else if (EPipelineType::PBR == mat->m_pipelineType)
            {
                ImGui::Text(mat->GetFilepath().c_str());
                ImGui::DragFloat3("Albedo", mat->m_diffuse.data, 0.1f);
                ImGui::DragFloat("Specular factor", &mat->m_specularFactor, 0.1f);
                ImGui::DragFloat("Metallic factor", &mat->m_metallicFactor, 0.1f);
                ImGui::DragFloat("Roughness factor", &mat->m_roughnessFactor, 0.1f);
                ImGui::DragFloat("Ambiant occlusion", &mat->m_ao, 0.1f);
                ImGui::DragFloat("Clear coat factor", &mat->m_clearCoatFactor, 0.1f);
                ImGui::DragFloat("Clear coat roughness factor", &mat->m_clearCoatRoughnessFactor, 0.1f);
            }
            
            if (ImGui::BeginTable("texturesTable", 2)) 
            {
                for (auto& [texType, texRef] : mat->m_textures)
                {
                    ImGui::TableNextColumn();
                    if (texType == ETextureType::ALBEDO)
                        ImGui::Text("Albedo");
                    else if (texType == ETextureType::AMBIENT)
                        ImGui::Text("Ambient");
                    else if (texType == ETextureType::NORMAL_MAP)
                        ImGui::Text("Normal Map");


                    ImGui::TableNextColumn();
                    TextureGenerator::GPUTextureBasic* GPUtex = nullptr;
                    if (texRef.get() && texRef->m_gpuTexture)
                    {
                        GPUtex = static_cast<TextureGenerator::GPUTextureBasic*>
                        (texRef->m_gpuTexture.get());
                    }

                    uint64_t texID = GPUtex ? (uint64_t)GPUtex->ID : 0u;

                    ImGui::ImageButton((ImTextureID)texID, { 50,50 });

                    if (ImGui::BeginDragDropTarget()) 
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
                        {
                            const char* c = (const char*)payload->Data;
                            std::string extension = String::ExtractValue(c, '.');

                            if (extension == "jpg" || extension == "png")
                            {   
                                mat->SetTexture(texType, c);
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
    }
}

void Inspector::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Inspector", &m_isOpened))
    {
        size_t selectedEntityCount = m_manager->m_selectedEntities.size();
        for (size_t i = 0u; i < selectedEntityCount; i++)
        {
            InspectComponents(m_manager->m_selectedEntities[i], static_cast<int>(i));
        }
        if (selectedEntityCount > 0)
        {
            if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                ImGui::OpenPopup("Add Component");
            }
        }


        ImVec2 center = { InputManager::GetInstance()->GetMousePos().x, InputManager::GetInstance()->GetMousePos().y };
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal("Add Component", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char search[32] = "";
            ImGui::InputText("##", search, IM_ARRAYSIZE(search));

            Behaviour* addBehaviour = nullptr;
            // TODO: Replace with list of available components
            if (ImGui::MenuItem("Transform"))           { addBehaviour = m_manager->m_selectedEntities[0]->AddBehaviour<Transform>(); }
            if (ImGui::MenuItem("Camera"))              { addBehaviour = m_manager->m_selectedEntities[0]->AddBehaviour<CameraComponent>(); }
            if (ImGui::MenuItem("Rigidbody"))           { addBehaviour = m_manager->m_selectedEntities[0]->AddBehaviour<Rigidbody>(); }
            if (ImGui::MenuItem("Box Collider"))        { addBehaviour = m_manager->m_selectedEntities[0]->AddBehaviour<BoxCollider>(); }
            if (ImGui::MenuItem("Sphere Collider"))     { addBehaviour = m_manager->m_selectedEntities[0]->AddBehaviour<SphereCollider>(); }
            if (ImGui::MenuItem("Capsule Collider"))    { addBehaviour = m_manager->m_selectedEntities[0]->AddBehaviour<CapsuleCollider>(); }
            if (ImGui::MenuItem("Light Component"))     { addBehaviour = m_manager->m_selectedEntities[0]->AddBehaviour<LightComponent>(); }
            if (ImGui::MenuItem("Portal Component"))	{ addBehaviour = m_manager->m_selectedEntities[0]->AddBehaviour<PortalComponent>(); }

            for (const std::string& name : CsScriptingSystem::GetInstance()->classesName) 
            {
                if (ImGui::MenuItem(name.c_str()))
                {
                    ScriptedBehaviour* script = m_manager->m_selectedEntities[0]->AddBehaviour<ScriptedBehaviour>();
                    script->SetScriptClass(name);
                    addBehaviour = script;
                }
            }   

            if (addBehaviour)
                addBehaviour->GetHost().Initialize();

            //---------------------------------------------------

            ImGui::SetItemDefaultFocus();
            if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}
