#include "panels/cell_system_displayer.hpp"

#include <imgui.h>

#include "scene_manager.hpp"

#include "camera.hpp"
#include "core/editor_manager.hpp"
#include "entity.hpp"
#include "transform.hpp"
#include "sky_renderer.hpp"
#include "utils.hpp"
#include "core/imcherry.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

CellSystemDisplayer::CellSystemDisplayer(EditorManager* manager): m_manager(manager)
{
    SceneManager* sceneManager = SceneManager::GetInstance();

    if (sceneManager->m_currentScene)
    {
        if (sceneManager->m_currentScene->m_cells.size() > 0)
            SelectCell(&sceneManager->m_currentScene->m_cells.begin()->second);
    }

    InputManager::PushContext("Editor Context");

    int i = 0;
    InputManager::AddActionAxes("FrontBack", i);
    InputManager::AddAxisToAction("FrontBack", { Keycode::W, Keycode::S });

    InputManager::AddActionAxes("RightLeft", i);
    InputManager::AddAxisToAction("RightLeft", { Keycode::D, Keycode::A });

    InputManager::AddActionSingle("Pick", i);
    InputManager::AddInputToAction("Pick", Keycode::LEFT_CLICK);

    InputManager::PopContext();
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

    }

    ImGui::PopStyleVar(1);
    ImGui::End();
}

void CellSystemDisplayer::SelectCell(Cell* cell)
{
    Cell* lastCell = m_displayedCell ? m_displayedCell : m_selectedCell;
    m_selectedCell = cell;

    if (m_displayedCell != m_selectedCell)
    {
        if (m_selectedCell && lastCell)
        {
            m_displayedCell = m_selectedCell;

            lastCell->RemoveViewer(m_camera);
            m_displayedCell->AddViewer(m_camera);
        }
        else if (m_selectedCell && !lastCell)
        {
            m_displayedCell = m_selectedCell;
            m_displayedCell->AddViewer(m_camera);
        }
    }
}

void CellSystemDisplayer::RenderCells()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::Dummy({ ImGui::GetContentRegionAvail().x , ImGui::GetContentRegionAvail().y * .01f });
    ImGui::BeginChild("CategoryList", {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * .8f } , false, window_flags);

    if (m_renameCell)
    {
        ImGui::OpenPopup("RenameCell");
        m_renameCell = false;
    }

    RenameCell();

    bool itemSelected = false;
    int i = 0;
    if (Scene* scene = SceneManager::GetInstance()->m_currentScene.get())
    {
        if (m_scene != scene)
        {
            m_selectedCell = nullptr;
            m_displayedCell = nullptr;
            m_scene = scene;
        }

        for (auto& cell : SceneManager::GetInstance()->m_currentScene->m_cells)
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
                        SelectCell(&cell.second);
                        itemSelected = true;
                        m_manager->RefreshHierarchy();
                    }

                    if (InputManager::GetInstance()->GetKeyDown(Keycode::RIGHT_CLICK))
                    {
                        ImGui::OpenPopup("options");
                        m_rightClickedCell = &cell.second;
                    }
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DROP"))
                    {
                        Entity* draggedEntity = (Entity*)payload->Data;

                        if (draggedEntity)
                        {
                            scene->MoveEntityFromCellToCell(draggedEntity->m_cell, &cell.second, draggedEntity);
                        }
                    }

                    ImGui::EndDragDropTarget();
                }

                ImGui::TreePop();
            }

            i++;
        }
    }
    ImGui::EndChild();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 12.f,0.f });
    ImGui::PushStyleColor(ImGuiCol_Button, { 0.166f, 0.166f, 0.221f, 1.000f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.427f, 0.427f, 0.456f, 1.000f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.220f, 0.205f, 0.235f, 1.000f });
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);


    if (ImGui::Button("Create new Cell", { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y}))
    {
        ImGui::PopStyleVar();
        ImCherryInternal::EndCherryHeader();
        ImGui::OpenPopup("AddCell");
    }
    else
    {
        ImGui::PopStyleVar();
        ImCherryInternal::EndCherryHeader();
    }

    CreateCell();
}

void CellSystemDisplayer::InvalidatePointers()
{
    m_scene = nullptr;
    m_displayedCell = nullptr;
    m_selectedCell = nullptr;
    m_rightClickedCell = nullptr;
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

            if (Scene* scene = SceneManager::GetInstance()->m_currentScene.get())
            {
                scene->AddCell(name);
            }

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
        std::string str = "Renaming ";
        str += m_rightClickedCell->GetName();
        str += " ? \n\n";
        ImGui::Text(str.c_str());
        ImGui::Separator();

        static char newName[32];
        IMGUI_LEFT_LABEL(ImGui::InputText, "Name:", newName, IM_ARRAYSIZE(newName));
        ImGui::Separator();

        if (ImGui::Button("Rename", ImVec2(120, 0)) && std::string(newName) != "")
        {
            if (Scene* scene = SceneManager::GetInstance()->m_currentScene.get())
            {
                scene->RenameCell(m_rightClickedCell->GetName(), newName);
            }

            ImGui::CloseCurrentPopup();

            memset(newName, 0, IM_ARRAYSIZE(newName));
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
            if (Scene* scene = SceneManager::GetInstance()->m_currentScene.get())
            {
                bool selected = m_rightClickedCell == m_displayedCell;

                if (selected)
                    m_rightClickedCell->RemoveViewer(m_camera);

                if (scene->RemoveCell(m_rightClickedCell->GetName()))
                {
                    if (selected)
                        m_displayedCell = nullptr;

                    ImGui::EndPopup();
                    return false;
                }
                else if (selected)
                    m_rightClickedCell->AddViewer(m_camera);
            }
		}

		if (ImGui::MenuItem("Rename"))
		{
            m_renameCell = true;
		}

        ImGui::EndPopup();
    }
    return true;
}