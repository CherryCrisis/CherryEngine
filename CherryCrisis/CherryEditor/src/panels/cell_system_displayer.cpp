#include "panels/cell_system_displayer.hpp"

#include "scene_manager.hpp"

#include <imgui.h>

#include "core/editor_manager.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "sky_renderer.hpp"
#include "utils.hpp"

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

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f,0.f });
    if (ImGui::Begin("Cell System", &m_isOpened))
    {
        static Entity* m_selectedEntity = nullptr;

        RenderCells();

        if (m_selectedCell)
        {
            RenderEntities();
        }


        ImGui::SameLine();
    }

    ImGui::PopStyleVar(1);
    ImGui::End();
    ImGui::ShowDemoWindow();
}

void CellSystemDisplayer::CellSettings()
{
    if (ImGui::TreeNode("Settings"))
    {
        std::string skyRendererPath;

        ImGui::InputText("SkyRenderer", skyRendererPath.data(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
            {
                const char* path = (const char*)payload->Data;
                std::string extension = String::ExtractValue(path, '.');

                if (textureExtensions.end() != textureExtensions.find("." + extension))
                {
                    if (std::shared_ptr<Texture> texture = ResourceManager::GetInstance()->GetResource<Texture>(path))
                    {
                        m_selectedCell->m_skyRenderer->SetTexture(texture);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::TreePop();
    }
}

void CellSystemDisplayer::RenderCells()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::BeginChild("CategoryList", ImVec2(ImGui::GetContentRegionAvail().x * 0.25f, ImGui::GetContentRegionAvail().y), false, window_flags);

    if (ImGui::Button("Add Cell"))
    {
        ImGui::OpenPopup("AddCell");
    }

    CreateCell();
    
    if (m_renameCell)
    {
        ImGui::OpenPopup("RenameCell");
        m_renameCell = false;
    }

    RenameCell();

    bool itemSelected = false;
    int i = 0;
    for (auto& cell : m_cellSystem->m_cells)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
        if (m_selectedCell == &cell.second) { flags |= ImGuiTreeNodeFlags_Selected; }

        if (ImGui::TreeNodeEx((void*)(intptr_t)i, flags, cell.first.c_str()))
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
                    Cell* lastCell = m_displayedCell ? m_displayedCell : m_selectedCell;
                    m_selectedCell = &cell.second;

                    if (m_displayedCell != m_selectedCell)
                    {
                        if (m_selectedCell && lastCell)
                        {
                            m_displayedCell = m_selectedCell;

                            lastCell->RemoveViewer(m_camera);
                            m_displayedCell->AddViewer(m_camera);
                        }
                    }

                    itemSelected = true;
                }

                if (InputManager::GetInstance()->GetKeyDown(Keycode::RIGHT_CLICK))
                {
                    ImGui::OpenPopup("options");
                    m_rightClickedCell = &cell.second;
                }
            }

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CELL_DROP"))
                {
                    m_cellSystem->MoveEntityFromCellToCell(m_selectedEntity->m_cell, &cell.second, m_selectedEntity);
                    m_selectedEntity = nullptr;
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
}

void CellSystemDisplayer::RenderEntities()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

	ImGui::BeginChild("CategoryFocus", ImVec2(0, ImGui::GetContentRegionAvail().y), true, window_flags);

    CellSettings();

	for (auto& entity : m_selectedCell->GetEntities())
	{
		Transform* entityTransform = entity->GetBehaviour<Transform>();
		if (entityTransform)
			if (entityTransform->GetParent())
				continue;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (m_selectedEntity == entity) { flags |= ImGuiTreeNodeFlags_Selected; }

		bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity->GetUUID(), flags, entity->GetName().c_str());

		if (InputManager::GetInstance()->GetKeyDown(Keycode::LEFT_CLICK) && ImGui::IsItemHovered())
		{
			m_selectedEntity = entity;
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("CELL_DROP", entity, sizeof(Entity), ImGuiCond_Once);
			ImGui::Text(entity->GetName().c_str());
			ImGui::EndDragDropSource();
		}

		if (opened)
			ImGui::TreePop();
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();
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

void CellSystemDisplayer::RenameCell()
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("RenameCell", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char name[32];
        IMGUI_LEFT_LABEL(ImGui::InputText, "Name:", name, IM_ARRAYSIZE(name));

        ImGui::Separator();

        if (ImGui::Button("Rename", ImVec2(120, 0)))
        {
            m_cellSystem->RenameCell(m_rightClickedCell->GetName(), name);

            ImGui::CloseCurrentPopup();

            memset(name, 0, IM_ARRAYSIZE(name));
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}


bool CellSystemDisplayer::Context()
{
    if (ImGui::BeginPopupContextItem("options"))
    {
        ImGui::Text("Actions ...");
        ImGui::Separator();

		if (ImGui::MenuItem("Delete"))
		{
			if (m_cellSystem->RemoveCell(m_rightClickedCell->GetName()))
			{
				ImGui::EndPopup();
				return false;
			}
		}

		if (ImGui::MenuItem("Rename"))
		{
            m_renameCell = true;
		}

		if (ImGui::MenuItem("Add Portal To Cell"))
		{
			//m_rightClickedCell->AddPortal();
		}

        ImGui::EndPopup();
    }
    return true;
}