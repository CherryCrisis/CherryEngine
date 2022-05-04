#include "panels/asset_browser.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "render_manager.hpp" //TODO: Erase when renderer manager instance is initialized in ENGINE
#include "core/editor_manager.hpp"
#include "resource_manager.hpp"
#include "input_manager.hpp"

#include "utils.hpp"
#include "resource.hpp"
#include "model_base.hpp"
#include "csassembly.hpp"
#include "command.hpp"
#include "model_loader.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

AssetBrowser::AssetBrowser(AssetSettingsDisplayer* assetSettingsDisplayer)
    : m_assetSettingsDisplayer(assetSettingsDisplayer)
{
    m_assetsDirectory = std::filesystem::current_path();
    m_assetsDirectory /= "Assets\\";

	QuerryBrowser();
}

void AssetBrowser::Render()
{
    if (!m_isOpened) return;

    if (ImGui::Begin("Browser", &m_isOpened, ImGuiWindowFlags_MenuBar))
    {
        //This order is very important !
        ContextCallback();

        RenderMenuBar();
        ResizeCell();
        RenderDirectoryBar();
        RenderNodes();

        BrowserAction();

    }
    ImGui::End();
}

void AssetBrowser::RenderMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        ImGui::InputText("Search", m_researchInput, IM_ARRAYSIZE(m_researchInput));
    }
    ImGui::EndMenuBar();
}

void AssetBrowser::ResizeCell()
{
    InputManager* inputManager = InputManager::GetInstance();

    if (inputManager->GetKey(Keycode::LEFT_CONTROL) && inputManager->GetMouseWheel().y != 0)
    {
        m_thumbnailSize += InputManager::GetInstance()->GetMouseWheel().y * 2;
    }
}

void AssetBrowser::RenderDirectoryBar()
{
    if (strlen(m_researchInput) > 0)
        return;

    std::deque<DirectoryNode*> directoryOrderNode;

    DirectoryNode* directory = m_currentDirectoryNode;
    while (directory)
    {
        if (directory)
            directoryOrderNode.push_front(directory);

        directory = directory->m_parentDirectory;
    }

    while (!directoryOrderNode.empty())
    {
        ImGui::Text("/");
        ImGui::SameLine();

        directory = directoryOrderNode.front();

        if (ImGui::Button(directory->m_filename.c_str()))
        {
            m_currentDirectoryNode = directory;
        }

        if (DragAndDropTarget(directory))
            return;

        directoryOrderNode.pop_front();

        if (!directoryOrderNode.empty())
            ImGui::SameLine();
    }

}

bool AssetBrowser::DragAndDropTarget(AssetNode* assetNode)
{
    //-- Drag and drop file or folder in folder --//
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
        {
            if (DirectoryNode* directoryNode = dynamic_cast<DirectoryNode*>(assetNode))
            {
                const char* draggedPath = static_cast<const char*>(payload->Data);

                std::string fullPath = m_assetsDirectoryNode->m_path.parent_path().parent_path().string() + "\\" + draggedPath;
                auto it = m_assetNodes.find(fullPath);

                if (it != m_assetNodes.end() && it->second.get() != assetNode && it->second.get()->m_path.parent_path().compare(directoryNode->m_path))
                {
                    AssetNode* draggedAssetNode = it->second.get();

                    std::string draggedAssetPath = draggedAssetNode->m_path.string();

                    if (DirectoryNode* draggedDirectoryNode = dynamic_cast<DirectoryNode*>(draggedAssetNode))
                    {
                        std::filesystem::path newPath = assetNode->m_path.string() + "\\" + draggedAssetNode->m_filename;

                        if (!std::filesystem::exists(newPath))
                            std::filesystem::create_directory(newPath);

                        CopyFolder(draggedAssetPath, newPath.string().c_str());

                        std::filesystem::remove_all(draggedDirectoryNode->m_path);
                    }
                    else
                    {
                        CopyFolder(draggedAssetPath, assetNode->m_path.string());
                        remove(draggedAssetPath.c_str());
                    }

                    std::string fullFilename(draggedAssetNode->m_filename + draggedAssetNode->m_extension);
                    draggedAssetNode->Rename((directoryNode->m_relativePath.string() + fullFilename).c_str());

                    QuerryBrowser();

                    ImGui::EndDragDropTarget();
                    return true;
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    return false;
}

void AssetBrowser::RenderNodes()
{
    float width = ImGui::GetContentRegionAvail().x;
    float cellSize = m_thumbnailSize + m_padding;
    int columnCount = (int)(width / cellSize);
    if (columnCount < 1) columnCount = 1;

    //Imgui : Only 64 columns allowed!
    if (columnCount >= 64)
        columnCount = 64;

    if (ImGui::BeginTable("assetNode", columnCount, ImGuiTableFlags_::ImGuiTableFlags_NoPadInnerX))
    {
        int columnID = columnCount;
        int assetID = 0;

        std::set<AssetNode*>& assetNodes = strlen(m_researchInput) > 0 ? m_allAssetNode : m_currentDirectoryNode->m_assetNodes;

        for (const auto& assetNode : assetNodes)
        {
            if (assetNode->m_previewTexture->GetResourceState() != EResourceState::LOADED)
            {
                ImGui::EndTable();
                return;
            }

            std::string fullFilename(assetNode->m_filename + assetNode->m_extension);
            //-- Research input --//
            if (strlen(m_researchInput) > 0)
            {
                std::string fullFilenameLower = String::ToLower(fullFilename);
                if (fullFilenameLower.find(String::ToLower(m_researchInput)) == std::string::npos)
                    continue;
            }

            ImGui::PushID(assetID);

            //-- Draw node --//
            {

                if (columnID >= columnCount)
                {
                    ImGui::TableNextRow();
                    columnID = 0;
                }

                ImGui::TableSetColumnIndex(columnID);
                columnID++;

                ImGuiContext& g = *GImGui;
                const ImGuiStyle& style = g.Style;
                ImGuiWindow* window = ImGui::GetCurrentWindow();
                const ImGuiID id = window->GetID("assetNode");
                float cellWidth = ImGui::GetContentRegionAvail().x;
                const char* filename = assetNode->m_filename.c_str();
                const ImVec2 label_size = ImGui::CalcTextSize(filename, NULL, true);
                ImVec2 textSize = ImGui::CalcItemSize({ cellWidth, 0 }, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

                //-- Highlight --//
                {
                    ImVec2 pos = window->DC.CursorPos;
                    ImVec2 size = ImGui::CalcItemSize({ cellWidth, m_upPadding + m_thumbnailSize + textSize.y * 2 }, 0, 0);

                    ImVec2 posSize(pos.x + size.x, pos.y + size.y);
                    ImRect bb(pos, posSize);
                    ImVec2 posMin(bb.Min.x, bb.Min.y);
                    ImVec2 posMax(bb.Max.x, bb.Max.y);

                    bool held;
                    bool pressed = ImGui::ButtonBehavior(bb, id, &assetNode->m_isHovered, &held, 0);

                    // Render
                    const ImU32 col = ImGui::GetColorU32((held && assetNode->m_isHovered) ? ImGuiCol_ButtonActive :
                        (m_focusedNode == assetNode || assetNode->m_isHovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

                    ImGui::RenderNavHighlight(bb, id);
                    ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

                }

                //-- Padding --//
                {
                    ImGui::Dummy({ 0, m_upPadding }); //Up padding

                    float leftPadding = (cellWidth - m_thumbnailSize) / 2 - 10;
                    ImGui::Dummy({ leftPadding, 0 }); //left padding for preview image
                    ImGui::SameLine();
                }

                //-- Draw preview image --//
                {
                    if (GPUTexturePreview* gpuTexturePreview = static_cast<GPUTexturePreview*>(assetNode->m_previewTexture->m_gpuTexture.get()))
                    {
                        ImGui::Image((void*)gpuTexturePreview->m_ID, { m_thumbnailSize, m_thumbnailSize }, { 0,1 }, { 1, 0 });
                    }
                }

                if (DragAndDropTarget(assetNode))
                {
                    ImGui::PopID();
                    ImGui::EndTable();
                    return;
                }

                //-- Text center alignment --//
                {
                    ImVec2 pos = window->DC.CursorPos;
                    pos.y += window->DC.CurrLineTextBaseOffset;

                    ImVec2 posSize(pos.x + textSize.x, pos.y + textSize.y);
                    ImRect bb(pos, posSize);
                    ImVec2 posMin(bb.Min.x + style.FramePadding.x, bb.Min.y + style.FramePadding.y);
                    ImVec2 posMax(bb.Max.x - style.FramePadding.x, bb.Max.y - style.FramePadding.y);

                    ImGui::ItemSize(textSize, style.FramePadding.y);
                    ImGui::ItemAdd(bb, id);

                    ImGui::RenderTextClipped(posMin, posMax, filename, NULL, &label_size, style.ButtonTextAlign, &bb);
                }
            }

            //-- Drag and drop asset from asset_browser to other --//
            if (ImGui::BeginDragDropSource())
            {
                std::string fullRelativePath = assetNode->m_relativePath.string() + fullFilename;

                ImGui::SetDragDropPayload("NODE", fullRelativePath.c_str(), fullRelativePath.size() + 1, ImGuiCond_Once);
                ImGui::Text(fullFilename.c_str());
                ImGui::EndDragDropSource();
            }

            //-- Action --//
            {
                if (assetNode->m_isHovered)
                {
                    if (!ImGui::IsMouseDragging(0))
                    {
                        ImGui::BeginTooltip();
                        ImGui::Text((assetNode->m_filename + assetNode->m_extension).c_str());
                        ImGui::EndTooltip();
                    }

                    if (ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) >= 1)
                    {
                        m_focusedNode = assetNode;
                    }

                    if (ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) >= 2)
                    {
                        assetNode->Action();
                    }

                    if (assetNode == m_focusedNode)
                    {
                        if (ImGui::IsKeyPressed(ImGuiKey_F2))
                        {
                            m_browserAction = EBrowserAction::RENAMING;
                        }

                        if (ImGui::IsKeyPressed(ImGuiKey_Delete))
                        {
                            m_browserAction = EBrowserAction::DELETING;
                        }
                    }
                }

            }

            ImGui::PopID();

            ++assetID;
        }
    }
    ImGui::EndTable();
}

void AssetBrowser::BrowserActionCreate()
{
    std::string popupName = std::format("Create {}", m_popupAssetType);
    ImGui::OpenPopup(popupName.c_str());

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(std::format("Create {}", m_popupAssetType).c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Separator();

        static char newName[32] = "New Name";
        IMGUI_LEFT_LABEL(ImGui::InputText, "New Name:", newName, IM_ARRAYSIZE(newName));

        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {

            if (!m_popupAssetType.compare("script"))
            {
                std::ofstream myfile;

                std::string filename(m_currentDirectoryNode->m_path.string());
                filename += "\\" + std::string(newName) + ".cs";

                myfile.open(filename);

                std::string scriptTemplateStr = scriptTemplate;
                String::ReplaceAll(scriptTemplateStr, "%s", newName);

                bool opened = false;
                if (myfile.is_open())
                {
                    myfile << scriptTemplateStr;
                    myfile.close();

                    EditorManager::SendNotification("Script Created", ENotifType::Success);
                }
            }
            else if (!m_popupAssetType.compare("folder"))
            {
                std::filesystem::path newPath = m_currentDirectoryNode->m_path;
                newPath /= newName;

                bool exist = std::filesystem::exists(newPath);
                int id = 0;

                while (exist)
                {
                    std::string path(std::format("{}{}", newPath.string(), id));

                    if (!(exist = std::filesystem::exists(path)))
                        newPath = path;

                    ++id;
                }

                std::filesystem::create_directory(newPath);

            }
            else if (!m_popupAssetType.compare("scene"))
            {
                std::filesystem::path newPath = m_currentDirectoryNode->m_path;
                newPath /= newName;

                bool exist = std::filesystem::exists(newPath.string() + ".cherry");
                int id = 0;

                while (exist)
                {
                    std::string path(std::format("{}{}", newPath.string(), id));

                    if (!(exist = std::filesystem::exists(path + ".cherry")))
                        newPath = path;

                    ++id;
                }

                newPath += ".cherry";

                std::ofstream myfile;

                myfile.open(String::ExtractValueStr(newPath.string(), "Assets\\"));
                if (myfile.is_open())
                {
                    myfile << " ";
                    myfile.close();
                }
            }
            else if (!m_popupAssetType.compare("material"))
            {
                std::filesystem::path newPath = m_currentDirectoryNode->m_path;
                newPath /= newName;

                bool exist = std::filesystem::exists(newPath.string() + ".mat");
                int id = 0;

                while (exist)
                {
                    std::string path(std::format("{}{}", newPath.string(), id));

                    if (!(exist = std::filesystem::exists(path + ".cherry")))
                        newPath = path;

                    ++id;
                }

                newPath += ".mat";

                std::shared_ptr<Material> material = ResourceManager::GetInstance()->AddResource<Material>(newPath.string().c_str(), true);
                CCImporter::SaveMaterial(material.get());
            }

            QuerryBrowser();
            ImGui::CloseCurrentPopup();
            m_browserAction = EBrowserAction::NONE;
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            m_browserAction = EBrowserAction::NONE;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void AssetBrowser::BrowserActionRename()
{
    if (!m_focusedNode)
        return;

    ImGui::OpenPopup("Rename");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        std::string str = "Renaming ";
        str += m_focusedNode->m_filename;
        str += " ? \n\n";

        ImGui::Text(str.c_str());

        ImGui::Separator();

        static char newName[32] = "New Name";
        IMGUI_LEFT_LABEL(ImGui::InputText, "New Name:", newName, IM_ARRAYSIZE(newName));

        if (ImGui::Button("Rename", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            std::filesystem::path newPath = m_currentDirectoryNode->m_path;
            newPath /= newName;

            std::string extension = m_focusedNode->m_extension;

            bool exist = std::filesystem::exists(newPath.string() + extension);
            int id = 0;

            while (exist)
            {
                std::string path(std::format("{}{}", newPath.string(), id));

                if (!(exist = std::filesystem::exists(path + extension)))
                    newPath = path;

                ++id;
            }

            newPath += extension;

            rename(m_focusedNode->m_path.string().c_str(), newPath.string().c_str());
            m_focusedNode->Rename(String::ExtractValueStr(newPath.string(), "Assets\\").c_str());

            QuerryBrowser();

            m_browserAction = EBrowserAction::NONE;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            m_browserAction = EBrowserAction::NONE;
            ImGui::CloseCurrentPopup();
        }

    }
    ImGui::EndPopup();
}

void AssetBrowser::BrowserActionDelete()
{
    if (!m_focusedNode)
        return;

    ImGui::OpenPopup("Delete");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Delete", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        std::string str = "Are you sure to delete ";
        str += m_focusedNode->m_filename;
        str += " ? \n\n";
        ImGui::Text("(IRREVERSIBLE!)");
        ImGui::Text(str.c_str());
        ImGui::Separator();

        if (ImGui::Button("Delete", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            if (DirectoryNode* directoryNode = dynamic_cast<DirectoryNode*>(m_focusedNode))
            {
                std::filesystem::remove_all(directoryNode->m_path);
            }
            else
            {
                remove(m_focusedNode->m_path.string().c_str());
            }

            m_focusedNode->Delete();
            QuerryBrowser();

            m_browserAction = EBrowserAction::NONE;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            m_browserAction = EBrowserAction::NONE;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void AssetBrowser::BrowserAction()
{
    switch (m_browserAction)
    {
    case EBrowserAction::CREATING:
        BrowserActionCreate();
        break;

    case EBrowserAction::RENAMING:
        BrowserActionRename();
        break;

    case EBrowserAction::DELETING:
        BrowserActionDelete();
        break;

    case EBrowserAction::NONE:
        return;
    }
}

void AssetBrowser::ContextCallback()
{
    static bool focusedNodeisHovered = false;
    if (ImGui::IsWindowHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Right) == 1)
    {
        focusedNodeisHovered = (m_focusedNode && m_focusedNode->m_isHovered) ? true : false;
        ImGui::OpenPopup("context");
    }

    if (ImGui::BeginPopupContextItem("context"))
    {
        ImGui::Text("Actions ...");
        ImGui::Separator();

        if (ImGui::BeginMenu("New"))
        {
            if (ImGui::MenuItem("Folder"))
            {
                m_popupAssetType = "folder";
                m_browserAction = EBrowserAction::CREATING;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("C# Script"))
            {
                m_popupAssetType = "script";
                m_browserAction = EBrowserAction::CREATING;
            }

            if (ImGui::MenuItem("Scene"))
            {
                m_popupAssetType = "scene";
                m_browserAction = EBrowserAction::CREATING;

            }

            if (ImGui::MenuItem("Material"))
            {
                m_popupAssetType = "material";
                m_browserAction = EBrowserAction::CREATING;

            }

            //TODO : Material

            ImGui::EndMenu();
        }

        if (focusedNodeisHovered)
        {
            ImGui::Separator();

            if (ImGui::MenuItem("Open"))    
                call(0, m_focusedNode->m_path.string().c_str());

            if (ImGui::MenuItem("Rename"))
                m_browserAction = EBrowserAction::RENAMING;

            if (ImGui::MenuItem("Delete"))
                m_browserAction = EBrowserAction::DELETING;

            if (ImGui::MenuItem("Reload"))
                m_focusedNode->Reload();

            ImGui::Separator();
        }

        if (ImGui::MenuItem("Open In Explorer"))
        {
            if (focusedNodeisHovered)
            {
                std::string str = "/select,";
                str += m_focusedNode->m_path.string();

                call("open", "explorer.exe", str.c_str());
            }
            else
                call(0, m_currentDirectoryNode->m_path.string().c_str());

        }

        ImGui::EndPopup();
    }
}

void AssetBrowser::SetAssetNode(const std::filesystem::path& path, AssetNode& assetNode)
{
    assetNode.m_path = path;
    assetNode.m_relativePath = String::ExtractValueStr(path.string(), "Assets\\");

    std::string filename = path.filename().string();
    assetNode.m_relativePath = String::ExtractKeyStr(assetNode.m_relativePath.string(), filename.c_str());
    assetNode.m_filename = String::ExtractLastKey(filename, '.');
    assetNode.m_extension = path.extension().string();

    assetNode.m_assetBrowser = this;
}

AssetBrowser::AssetNode* AssetBrowser::RecursiveQuerryBrowser(const std::filesystem::path& m_path, DirectoryNode* parentDirectory)
{
    ResourceManager* resourceManager(ResourceManager::GetInstance());

    if (std::filesystem::is_directory(m_path))
    {
        DirectoryNode directoryNode;
        SetAssetNode(m_path, directoryNode);

        directoryNode.m_parentDirectory = parentDirectory;

        directoryNode.m_previewTexture = resourceManager->AddResource<Texture>("Internal/Icons/folder_icon.png", true);

        auto directory_iterator = std::filesystem::directory_iterator(m_path);

        auto pair = m_assetNodes.insert({ directoryNode.m_path.string(), std::make_unique<DirectoryNode>(directoryNode)});
        AssetNode* assetNode = pair.first->second.get();

        for (const std::filesystem::directory_entry& entry : directory_iterator)
        {
            DirectoryNode* directoryNodePtr = static_cast<DirectoryNode*>(assetNode);
            directoryNodePtr->m_assetNodes.insert(RecursiveQuerryBrowser(entry.path(), directoryNodePtr));
        }

        return assetNode;
    }

    if (m_path.has_extension())
    {
        std::string extension = m_path.extension().string();


        //-- Texture --//
        {
            auto it = textureExtensions.find(extension);
            if (it != textureExtensions.end())
            {
                TextureNode textureNode;
                SetAssetNode(m_path, textureNode);

                std::string filepath(textureNode.m_relativePath.string() + textureNode.m_filename + textureNode.m_extension);
                textureNode.m_resource = resourceManager->AddResource<Texture>(filepath.c_str(), true);

                textureNode.m_previewTexture = std::dynamic_pointer_cast<Texture>(textureNode.m_resource);

                auto pair = m_assetNodes.insert({ textureNode.m_path.string(), std::make_unique<TextureNode>(textureNode) });
                return pair.first->second.get();
            }
        }

        //-- Model --//
        {
            auto it = modelExtensions.find(extension);
            if (it != modelExtensions.end())
            {
                ModelNode modelNode;
                SetAssetNode(m_path, modelNode);

                std::string filepath(modelNode.m_relativePath.string() + modelNode.m_filename + modelNode.m_extension);
                std::shared_ptr<ModelBase> modelBase = resourceManager->AddResource<ModelBase>(filepath.c_str(), true);
                modelNode.m_resource = modelBase;

                modelNode.m_previewTexture = resourceManager->AddResource<Texture>("Internal/Icons/model_icon.png", true);

                auto pair = m_assetNodes.insert({ modelNode.m_path.string(), std::make_unique<ModelNode>(modelNode) });
                AssetNode* assetNode = pair.first->second.get();

                modelBase->m_OnReloaded.Bind(&AssetBrowser::ResourceAssetNode<ModelBase>::ReloadPreviewTexture, static_cast<ResourceAssetNode<ModelBase>*>(assetNode));

                return assetNode;
            }
        }

        //-- Shader --//
        {
            auto it = shaderExtensions.find(extension);
            if (it != shaderExtensions.end())
            {
                ShaderNode shaderNode;
                SetAssetNode(m_path, shaderNode);

                std::string filepath(shaderNode.m_relativePath.string() + shaderNode.m_filename + shaderNode.m_extension);
                std::shared_ptr<Shader> shader = resourceManager->AddResource<Shader>(filepath.c_str(), true);
                shaderNode.m_resource = shader;

                shaderNode.m_previewTexture = resourceManager->AddResource<Texture>("Internal/Icons/shader_icon.png", true);

                auto pair = m_assetNodes.insert({ shaderNode.m_path.string(), std::make_unique<ShaderNode>(shaderNode) });
                return pair.first->second.get();
            }
        }

        //-- Script --//
        if (!scriptExtensions.compare(extension))
        {
            ScriptNode scriptNode;
            SetAssetNode(m_path, scriptNode);

            std::string filepath(scriptNode.m_relativePath.string() + scriptNode.m_filename + scriptNode.m_extension);

            scriptNode.m_previewTexture = resourceManager->AddResource<Texture>("Internal/Icons/script_icon.png", true);

            auto pair = m_assetNodes.insert({ scriptNode.m_path.string(), std::make_unique<ScriptNode>(scriptNode) });
            return pair.first->second.get();
        }

        //-- Scene --//
        if (!sceneExtensions.compare(extension))
        {
            EmptyNode emptyNode;
            SetAssetNode(m_path, emptyNode);

            emptyNode.m_previewTexture = resourceManager->AddResource<Texture>("Internal/Icons/scene_icon.png", true);

            auto pair = m_assetNodes.insert({ emptyNode.m_path.string(), std::make_unique<EmptyNode>(emptyNode) });
            return pair.first->second.get();
        }

        //-- Material --//
        {
            if (!matExtensions.compare(extension))
            {
                MaterialNode materialNode;
                SetAssetNode(m_path, materialNode);

                std::string filepath(materialNode.m_relativePath.string() + materialNode.m_filename + materialNode.m_extension);
                std::shared_ptr<Material> material = resourceManager->AddResource<Material>(filepath.c_str(), true);
                materialNode.m_resource = material;

                materialNode.m_previewTexture = resourceManager->AddResource<Texture>("Internal/Icons/file_icon.png", true);

                auto pair = m_assetNodes.insert({ materialNode.m_path.string(), std::make_unique<MaterialNode>(materialNode) });
                return pair.first->second.get();
            }
        }

        //-- Sound --//
        /*
        {
            auto it = textureExtensions.find(extension);
            if (it != textureExtensions.end())
            {
                SoundNode soundNode;
                SetAssetNode(m_path, soundNode);

                std::string filepath(soundNode.m_relativePath.string() + soundNode.m_filename + soundNode.m_extension);
                soundNode.m_resource = resourceManager->AddResource<Sound>(filepath.c_str(), true);

                soundNode.m_icon = resourceManager->AddResource<Texture>("Internal/Icons/sound_icon.png", true);

                auto pair = m_allAssetNode.insert(std::make_unique<SoundNode>(soundNode));
                return pair.first->get();
            }
        }*/
        
    }

    EmptyNode emptyNode;
    SetAssetNode(m_path, emptyNode);

    emptyNode.m_previewTexture = resourceManager->AddResource<Texture>("Internal/Icons/file_icon.png", true);

    auto pair = m_assetNodes.insert({ emptyNode.m_path.string(), std::make_unique<EmptyNode>(emptyNode)});
    return pair.first->second.get();
}

void AssetBrowser::QuerryBrowser()
{
	m_currentDirectoryNode = nullptr;
	m_assetsDirectoryNode = nullptr;
	m_assetNodes.clear();
    m_allAssetNode.clear();

    RenderManager::GetInstance(); //TODO: Erase when renderer manager instance is initialized in ENGINE

    if (std::filesystem::exists(m_assetsDirectory))
    {
       AssetNode* assetNode = RecursiveQuerryBrowser(m_assetsDirectory, nullptr);
       m_assetsDirectoryNode = dynamic_cast<DirectoryNode*>(assetNode);
       m_assetsDirectoryNode->m_filename = "Assets";
       m_currentDirectoryNode = m_assetsDirectoryNode;
    }

    for (const auto& assetNode : m_assetNodes)
    {
        assetNode.second->UploadPreviewTexture();
        m_allAssetNode.insert(assetNode.second.get());
    }

    ResourceManager::GetInstance()->Purge();
}

void AssetBrowser::AssetNode::UploadPreviewTexture()
{
    if (!m_previewTexture)
        return;

    if (!m_previewTexture->GetData() || m_previewTexture->GetWidth() <= 0 || m_previewTexture->GetHeight() <= 0)
        return;

    m_previewTexture->m_gpuTexture = std::make_unique<GPUTexturePreview>(m_previewTexture.get());
}

void AssetBrowser::GPUTexturePreview::Generate(Texture* texture)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

    glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ETextureFormat textureFormat = texture->GetInternalFormat();

    if (textureFormat == ETextureFormat::RGB || textureFormat == ETextureFormat::RGBA)
    {
        unsigned int internalFormat = GL_RGBA8;
        if (textureFormat == ETextureFormat::RGB)
            internalFormat = GL_RGB8;

        glTextureStorage2D(m_ID, 1, internalFormat, texture->GetWidth(), texture->GetHeight());
        glTextureSubImage2D(m_ID, 0, 0, 0, texture->GetWidth(), texture->GetHeight(), (unsigned int)textureFormat, GL_UNSIGNED_BYTE, texture->GetData());

        glGenerateTextureMipmap(m_ID);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, m_ID);

        int mipmapsCount = texture->GetMipmapCount();
        int width = texture->GetWidth();
        int height = texture->GetHeight();
        int offset = 0;
        unsigned char* data = (unsigned char*)texture->GetData();

        for (int mipmapId = 0; mipmapId < mipmapsCount && (width || height); ++mipmapId)
        {
            CCMaths::Min(width, 1);
            CCMaths::Min(height, 1);

            int size = ((width + 3) / 4) * ((height + 3) / 4) * texture->GetBlockSize();
            glCompressedTexImage2D(GL_TEXTURE_2D, mipmapId, (unsigned int)textureFormat,
                width, height, 0, size, data + offset);


            offset += size;
            width >>= 1;
            height >>= 1;
        }
    }

    //texture->ClearData();
}

void AssetBrowser::GPUTexturePreview::Regenerate(Texture* texture)
{
    Destroy();
    Generate(texture);
}

void AssetBrowser::GPUTexturePreview::Destroy()
{
    glDeleteTextures(1, &m_ID);
}

AssetBrowser::GPUTexturePreview::GPUTexturePreview(Texture* texture)
{
    texture->m_OnReloaded.Bind(&GPUTexturePreview::OnReload, this);
    Generate(texture);
}

AssetBrowser::GPUTexturePreview::~GPUTexturePreview()
{
    Destroy();
}

void AssetBrowser::GPUTexturePreview::OnReload(std::shared_ptr<Texture> texture)
{
    Regenerate(texture.get());
}