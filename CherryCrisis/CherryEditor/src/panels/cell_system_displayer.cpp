
#include "panels/cell_system_displayer.hpp"

#include "scene_manager.hpp"

#include <imgui.h>
#include <comdef.h>

#include "time_manager.hpp"
#include "core/editor_manager.hpp"
#include "transform.hpp"
#include "maths.hpp"
#include "resource_manager.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

CellSystemDisplayer::CellSystemDisplayer()
{
    m_cellSystem = CellSystem::GetInstance();

    InputManager* IM = InputManager::GetInstance();
    IM->PushContext("Editor Context");

    int i = 0;
    IM->AddActionAxes("FrontBack", i);
    IM->AddAxisToAction("FrontBack", { Keycode::W, Keycode::S });

    IM->AddActionAxes("RightLeft", i);
    IM->AddAxisToAction("RightLeft", { Keycode::D, Keycode::A });

    IM->AddActionButtons("Pick", i);
    IM->AddInputToAction("Pick", Keycode::LEFT_CLICK);

    IM->PopContext();
}

void CellSystemDisplayer::UpdateCamera()
{
}

void CellSystemDisplayer::Render()
{
    if (!m_isOpened)
        return;

    m_isActive = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f,0.f });
    if (ImGui::Begin("Cell System", &m_isOpened))
    {
        static Entity* selectedEntity = nullptr;

        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            ImGui::BeginChild("CategoryList", ImVec2(ImGui::GetContentRegionAvail().x * 0.25f, ImGui::GetContentRegionAvail().y), false, window_flags);

            if (ImGui::Button("Add Cell"))
            {
                ImGui::OpenPopup("AddCell");
            }

            CreateCell();

            bool itemSelected = false;
            int i = 0;
            for (auto& cell : m_cellSystem->m_cells)
            {
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
                if (m_selectedCell == &cell.second) { flags |= ImGuiTreeNodeFlags_Selected; }

                if (ImGui::TreeNodeEx((void*)i, flags, cell.first.c_str()))
                {
                    if (!Context())
                    {
                        if (m_selectedCell == m_rightClickedCell)
                            m_selectedCell = nullptr;

                        m_rightClickedCell = nullptr;

                        ImGui::TreePop();
                        break;
                    }

                    if (ImGui::IsItemHovered())
                    {
                        if (InputManager::GetInstance()->GetKeyDown(Keycode::LEFT_CLICK))
                        {
                            m_selectedCell = &cell.second;
                            m_selectedCategory = i;
                            itemSelected = true;
                        }

                        if (InputManager::GetInstance()->GetKeyDown(Keycode::RIGHT_CLICK))
                        {
                            ImGui::OpenPopup("context");
                            m_rightClickedCell = &cell.second;
                        }
                    }

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DROP"))
                        {
                            m_cellSystem->MoveEntityFromCellToCell(selectedEntity->m_cell, &cell.second, selectedEntity);
                            selectedEntity = nullptr;
                        }

                        ImGui::EndDragDropTarget();
                    }

                    ImGui::TreePop();
                }

                i++;
            }

            if (!itemSelected && ImGui::IsWindowHovered() && InputManager::GetInstance()->GetKeyDown(Keycode::LEFT_CLICK))
            {
                m_selectedCell = nullptr;
            }

            ImGui::EndChild(); ImGui::SameLine();

            if (m_selectedCell)
            {
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

                ImGui::BeginChild("CategoryFocus", ImVec2(0, ImGui::GetContentRegionAvail().y), true, window_flags);

                for (auto& entity : m_selectedCell->GetEntities())
                {
                    Transform* entityTransform = entity->GetBehaviour<Transform>();
                    if (entityTransform)
                        if (entityTransform->GetParent())
                            continue;

                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

                    if (selectedEntity == entity) { flags |= ImGuiTreeNodeFlags_Selected; }

                    bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity->GetUUID(), flags, entity->GetName().c_str());

                    if (InputManager::GetInstance()->GetKeyDown(Keycode::LEFT_CLICK) && ImGui::IsItemHovered())
                    {
                        selectedEntity = entity;
                    }

                    if (ImGui::BeginDragDropSource())
                    {
                        ImGui::SetDragDropPayload("HIERARCHY_DROP", entity, sizeof(Entity), ImGuiCond_Once);
                        ImGui::Text(entity->GetName().c_str());
                        ImGui::EndDragDropSource();
                    }

                    if (opened)
                        ImGui::TreePop();
                }

                ImGui::EndChild();
                ImGui::PopStyleVar();
            }
        }
        ImGui::SameLine();
    }

    ImGui::PopStyleVar(1);
    ImGui::End();
}

void CellSystemDisplayer::CreateCell()
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("AddCell", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        std::string str = "Name the new Cell: \n\n";
        ImGui::Text(str.c_str());
        ImGui::Separator();

        static char name[32];
        IMGUI_LEFT_LABEL(ImGui::InputText, "Name:", name, IM_ARRAYSIZE(name));

        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();

            m_cellSystem->AddOrGetCell(name);

            memset(name, 0, IM_ARRAYSIZE(name));
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}

void CellSystemDisplayer::Focus()
{
    m_isFocused = true;
    m_inputs->SetCursorHidden();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    ImGui::SetWindowFocus("Scene");
}

void CellSystemDisplayer::Unfocus()
{
    m_isFocused = false;
    m_inputs->SetCursorDisplayed();
    ImGui::GetIO().ConfigFlags = ImGui::GetIO().ConfigFlags & ~ImGuiConfigFlags_NoMouse;
}

bool CellSystemDisplayer::Context()
{
    if (ImGui::BeginPopupContextItem("context"))
    {
        ImGui::Text("Actions ...");
        ImGui::Separator();

        if (ImGui::Button("Delete"))
        {
            if (m_cellSystem->RemoveCell(m_rightClickedCell->GetName()))
            {
                ImGui::EndPopup();
                return false;
            }
        }

        if (ImGui::Button("Add Poartal To Cell"))
        {
            m_rightClickedCell->AddPortal();
        }

        //if (ImGui::BeginMenu("New"))
        //{

        //    if (ImGui::MenuItem("Empty"))
        //    {
        //        Entity* empty = new Entity("Empty");
        //        SceneManager::GetInstance()->m_currentScene->AddEntity(empty);
        //    }
        //    if (ImGui::MenuItem("ModelRenderer")) {}
        //    if (ImGui::MenuItem("Camera")) {}
        //    if (ImGui::MenuItem("Particle System")) {}
        //    if (ImGui::MenuItem("Audio Source")) {}
        //    if (ImGui::BeginMenu("Shapes"))
        //    {
        //        if (ImGui::MenuItem("Cube"))
        //        {
        //            Entity* cube = new Entity("Cube");
        //            std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedCube");
        //            Mesh::CreateCube(mesh, 1, 1, 1);
        //            std::shared_ptr<Model> model = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedCube", true, mesh);
        //            Transform* tr = cube->AddBehaviour<Transform>();
        //            ModelRenderer* rdr = cube->AddBehaviour<ModelRenderer>();
        //            rdr->m_transform = tr;
        //            rdr->SetModel(model);
        //            SceneManager::GetInstance()->m_currentScene->AddEntity(cube);
        //            m_manager->FocusEntity(cube);
        //        }
        //        if (ImGui::MenuItem("Sphere"))
        //        {
        //            Entity* cube = new Entity("Sphere");
        //            std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedSphere");
        //            Mesh::CreateCube(mesh, 1, 1, 1);
        //            std::shared_ptr<Model> model = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedSphere", true, mesh);
        //            Transform* tr = cube->AddBehaviour<Transform>();
        //            ModelRenderer* rdr = cube->AddBehaviour<ModelRenderer>();
        //            rdr->m_transform = tr;
        //            rdr->SetModel(model);
        //            SceneManager::GetInstance()->m_currentScene->AddEntity(cube);
        //            m_manager->FocusEntity(cube);
        //        }
        //        if (ImGui::MenuItem("Cone"))
        //        {
        //            Entity* cube = new Entity("Cone");
        //            std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedCone");
        //            Mesh::CreateCube(mesh, 1, 1, 1);
        //            std::shared_ptr<Model> model = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedCone", true, mesh);
        //            Transform* tr = cube->AddBehaviour<Transform>();
        //            ModelRenderer* rdr = cube->AddBehaviour<ModelRenderer>();
        //            rdr->m_transform = tr;
        //            rdr->SetModel(model);
        //            SceneManager::GetInstance()->m_currentScene->AddEntity(cube);
        //            m_manager->FocusEntity(cube);
        //        }
        //        if (ImGui::MenuItem("Plane"))
        //        {
        //            Entity* cube = new Entity("Plane");
        //            std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedPlane");
        //            Mesh::CreateCube(mesh, 1, 1, 1);
        //            std::shared_ptr<Model> model = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedPlane", true, mesh);
        //            Transform* tr = cube->AddBehaviour<Transform>();
        //            ModelRenderer* rdr = cube->AddBehaviour<ModelRenderer>();
        //            rdr->m_transform = tr;
        //            rdr->SetModel(model);
        //            SceneManager::GetInstance()->m_currentScene->AddEntity(cube);
        //            m_manager->FocusEntity(cube);
        //        }
        //        ImGui::EndMenu();
        //    }
        //    ImGui::Separator();

        //    ImGui::EndMenu();
        //}
        //if (m_manager->m_selectedEntities.size() > 0)
        //{
        //    ImGui::Separator();

        //    if (ImGui::MenuItem("Delete"))
        //    {
        //        for (auto& entity : m_manager->m_selectedEntities)
        //        {
        //            SceneManager::GetInstance()->m_currentScene->RemoveEntity(entity);
        //            //To Change
        //            m_manager->m_selectedEntities.clear();
        //        }
        //    }

        //    if (ImGui::MenuItem("Copy")) {}

        //    ImGui::Separator();
        //}
        //if (ImGui::MenuItem("Paste")) {}

        ImGui::EndPopup();
        return true;
    }
}