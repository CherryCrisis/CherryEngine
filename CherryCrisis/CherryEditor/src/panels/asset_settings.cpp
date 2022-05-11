#include "Panels/asset_settings.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "resource_manager.hpp"

#include "texture.hpp"
#include "material.hpp"
#include "spheremap.hpp"
#include "cubemap.hpp"
#include "utils.hpp"
#include "Panels/asset_browser.hpp"

void AssetSettingsDisplayer::Render()
{
    if (!m_isOpened) 
        return;

    if (ImGui::Begin("Asset Settings", &m_isOpened))
    {
        if (m_assetSettings)
            m_assetSettings->Render();
    }
    ImGui::End();

}

#pragma region Texture
const std::map<const ETextureFormat, const int> IdOfTextureFormat
{
    {ETextureFormat::RGB, 0},
    {ETextureFormat::RGBA, 1},
    {ETextureFormat::DXT1, 2},
    {ETextureFormat::DXT1a, 3},
    {ETextureFormat::DXT3, 4},
    {ETextureFormat::DXT5, 5},
    {ETextureFormat::DXT6, 6},
    {ETextureFormat::DXT7, 7},
};

const std::map<const int, const ETextureFormat> textureFormatByID
{
    {0, ETextureFormat::RGB},
    {1, ETextureFormat::RGBA},
    {2, ETextureFormat::DXT1},
    {3, ETextureFormat::DXT1a},
    {4, ETextureFormat::DXT3},
    {5, ETextureFormat::DXT5},
    {6, ETextureFormat::DXT6},
    {7, ETextureFormat::DXT7},
};

TextureSettings::TextureSettings(std::shared_ptr<Texture> texture)
    : m_texture(texture)
{
    if (!m_texture)
        return;

    m_currentId = IdOfTextureFormat.at(m_texture->GetInternalFormat());
    m_isFlipped = m_texture->GetIsFlipped();
}

void TextureSettings::Render()
{
        ImGui::Separator();
        ImGui::Text(std::format("Texture name : {}", m_texture->GetFilesystemPath()->filename().string()).c_str());
        ImGui::Separator();

        const char* label = "Texture Format";
        const char* list[] = { "RGB", "RGBA", "DXT1", "DXT1a", "DXT3", "DXT5", "DXT6", "DXT7" };

        if (ImGui::BeginCombo(label, list[m_currentId]))
        {
            for (int n = 0; n < IdOfTextureFormat.size(); n++)
            {
                const bool is_selected = (m_currentId == n);
                if (ImGui::Selectable(list[n], is_selected))
                {
                    m_settingsChanged = true;
                    m_currentId = n;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
        ImGui::EndCombo();
        }

        if (ImGui::Checkbox("Flip", &m_isFlipped))
            m_settingsChanged = true;

        if (m_settingsChanged)
        {
            if (ImGui::Button("Apply"))
            {
                m_texture->SetInternalFormat(textureFormatByID.at(m_currentId));
                Resource<Texture>::ReloadResource(m_texture, m_isFlipped);
                m_settingsChanged = false;
            }
        }
}

#pragma endregion

#pragma region Material
const std::vector<std::string> TextureTypeToStr
{
    {"AMBIENT"},
    {"ALBEDO"},
    {"NORMAL_MAP"},
    {"METALLIC"},
    {"ROUGHNESS"},
    {"SPECULAR"},
    {"AO"},
};

MaterialSettings::MaterialSettings(std::shared_ptr<Material> material)
    : m_material(material)
{
    if (!m_material)
        return;

    GenerateTextureList();
}

void MaterialSettings::GenerateTextureList()
{
    m_textures.clear();
    int textureCount = static_cast<int>(ETextureType::COUNT);
    for (int textureTypeID = 0; textureTypeID < textureCount; ++textureTypeID)
    {
        ETextureType currentTypeID = static_cast<ETextureType>(textureTypeID);
        auto pair = m_material->m_textures.find(currentTypeID);
    
        if (pair != m_material->m_textures.end())
        {
            m_textureGenerator.Generate(pair->second.get());
            m_textures.emplace(pair->first, pair->second);
        }
        else
        {
            m_textures.emplace(currentTypeID, nullptr);
        }
    }
}

void MaterialSettings::Render()
{
    ImGui::Separator();
    ImGui::Text(std::format("Material name : {}", m_material->GetFilesystemPath()->filename().string()).c_str());
    ImGui::Separator();

    const char* label = "Pipeline";
    const char* list[] = { "LIT", "PBR" };

    int currentPipeline = static_cast<int>(m_material->m_pipelineType);
    if (ImGui::BeginCombo(label, list[currentPipeline]))
    {
        for (int n = 0; n < std::size(list); n++)
        {
            const bool is_selected = (currentPipeline == n);
            if (ImGui::Selectable(list[n], is_selected))
            {
                m_settingsChanged = true;
                m_material->m_pipelineType = static_cast<EPipelineType>(n);
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    if (EPipelineType::LIT == static_cast<EPipelineType>(currentPipeline))
    {
        m_settingsChanged += ImGui::DragFloat3("Ambient", m_material->m_ambient.data, 0.1f, 0.f) +
            ImGui::DragFloat3("Diffuse", m_material->m_diffuse.data, 0.1f, 0.f) +
            ImGui::DragFloat3("Specular", m_material->m_specular.data, 0.1f, 0.f) +
            ImGui::DragFloat3("Emissive", m_material->m_emissive.data, 0.1f, 0.f) +
            ImGui::DragFloat("Shininess", &m_material->m_shininess, 0.1f, 0.f);
    }
    else if (EPipelineType::PBR == static_cast<EPipelineType>(currentPipeline))
    {
        m_settingsChanged += ImGui::DragFloat3("Albedo", m_material->m_diffuse.data, 0.1f, 0.f) +
            ImGui::DragFloat("Specular factor", &m_material->m_specularFactor, 0.1f, 0.f, 1.f) +
            ImGui::DragFloat("Metallic factor", &m_material->m_metallicFactor, 0.1f, 0.f, 1.f) +
            ImGui::DragFloat("Roughness factor", &m_material->m_roughnessFactor, 0.1f, 0.f, 1.f) +
            ImGui::DragFloat("Ambiant occlusion", &m_material->m_ao, 0.1f, 0.f, 1.f) +
            ImGui::DragFloat("Clear coat factor", &m_material->m_clearCoatFactor, 0.1f, 0.f, 1.f) +
            ImGui::DragFloat("Clear coat roughness factor", &m_material->m_clearCoatRoughnessFactor, 0.1f, 0.f, 1.f) +
            ImGui::Checkbox("Use normal map", &m_material->m_hasNormal);
    }

    if (ImGui::BeginTable("texturesTable", 2))
    {
        for (auto& [texType, texRef] : m_textures)
        {
            ImGui::TableNextColumn();
            ImGui::Text(TextureTypeToStr[static_cast<int>(texType)].c_str());
            ImGui::TableNextColumn();

            uint64_t texID = 0u;
            if (texRef)
            {
                if (auto gpuTextureBasic = static_cast<TextureGenerator::GPUTextureBasic*>(texRef->m_gpuTexture.get()))
                    texID = gpuTextureBasic->ID;
            }

            //-- Highlight --//
            {
                ImGuiContext& g = *GImGui;
                const ImGuiStyle& style = g.Style;
                ImGuiWindow* window = ImGui::GetCurrentWindow();
                const ImGuiID id = window->GetID("texturesTable");
                float cellWidth = ImGui::GetContentRegionAvail().x;
                ImVec2 pos = window->DC.CursorPos;
                ImVec2 size = ImGui::CalcItemSize({ 50, 50 }, 0, 0);

                ImVec2 posSize(pos.x + size.x, pos.y + size.y);
                ImRect bb(pos, posSize);
                ImVec2 posMin(bb.Min.x, bb.Min.y);
                ImVec2 posMax(bb.Max.x, bb.Max.y);

                bool held, hovered;
                bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

                if (hovered)
                    m_textureTypeFoccused = static_cast<int>(texType);

                // Render
                const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive :
                    hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

                ImVec4 imageColor = hovered == true ? ImVec4{ 1, 1, 1, 1 } : ImVec4{ 0.5f, 0.5f, 0.5f, 1 };
                ImGui::Image((ImTextureID)texID, { 50,50 }, { 0,0 }, { 1,1 }, { 1, 1, 1, 1 }, imageColor);
            }


            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
                {
                    const char* texturePath = (const char*)payload->Data;
                    std::string extension = String::ExtractValue(texturePath, '.');

                    auto it = textureExtensions.find("." + extension);
                    if (it != textureExtensions.end())
                    {
                        auto textureIt = m_textures.find(texType);
                        if (textureIt != m_textures.end())
                        {
                            if (std::shared_ptr<Texture> texture = ResourceManager::GetInstance()->GetResource<Texture>(texturePath))
                            {
                                textureIt->second = texture;

                                m_material->m_textures = m_textures;
                                m_settingsChanged = true;
                            }
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
        }
        ImGui::EndTable();
    }

    if (m_settingsChanged)
    {
        //true for save only
        Resource<Material>::ReloadResource(m_material, true);
        GenerateTextureList();
        m_settingsChanged = false;
    }

    ContextCallback();
}

void MaterialSettings::ContextCallback()
{
    static bool focusedNodeisHovered = false;
    if (ImGui::IsWindowHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Right) == 1)
    {
        ImGui::OpenPopup("context");
    }

    if (ImGui::BeginPopupContextItem("context"))
    {
        ImGui::Text("Actions ...");
        ImGui::Separator();

        if (ImGui::MenuItem("Clear"))
        {
            auto textureIt = m_textures.find(static_cast<ETextureType>(m_textureTypeFoccused));
            if (textureIt != m_textures.end())
            {
                textureIt->second = nullptr;

                m_material->m_textures = m_textures;
                m_settingsChanged = true;
            }
        }

        if (ImGui::MenuItem("Texture Settings"))
        {
            auto textureIt = m_textures.find(static_cast<ETextureType>(m_textureTypeFoccused));
            if (textureIt != m_textures.end())
            {
                if (textureIt->second)
                {
                    m_assetSettingsDisplayer->SetAssetSettings(new TextureSettings(textureIt->second));
                }
            }
        }

        ImGui::EndPopup();
    }
}

#pragma endregion

#pragma region Spheremap

SpheremapSettings::SpheremapSettings(std::shared_ptr<Spheremap> spheremap)
    : m_spheremap(spheremap)
{
}

void SpheremapSettings::Render()
{
    ImGui::Separator();
    ImGui::Text(std::format("Spheremap name : {}", m_spheremap->GetFilesystemPath()->filename().string()).c_str());
    ImGui::Separator();

    if (ImGui::BeginTable("texturesTable", 2))
    {
        ImGui::TableNextColumn();
        ImGui::Text("texture");
        ImGui::TableNextColumn();

        uint64_t texID = 0u;
        if (m_spheremap->m_texture)
        {
            if (auto gpuTextureBasic = static_cast<TextureGenerator::GPUTextureBasic*>(m_spheremap->m_texture->m_gpuTexture.get()))
                texID = gpuTextureBasic->ID;
        }

        //-- Highlight --//
        {
            ImGuiContext& g = *GImGui;
            const ImGuiStyle& style = g.Style;
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            const ImGuiID id = window->GetID("texturesTable");
            float cellWidth = ImGui::GetContentRegionAvail().x;
            ImVec2 pos = window->DC.CursorPos;
            ImVec2 size = ImGui::CalcItemSize({ 50, 50 }, 0, 0);

            ImVec2 posSize(pos.x + size.x, pos.y + size.y);
            ImRect bb(pos, posSize);
            ImVec2 posMin(bb.Min.x, bb.Min.y);
            ImVec2 posMax(bb.Max.x, bb.Max.y);

            bool held;
            bool pressed = ImGui::ButtonBehavior(bb, id, &isHovered, &held, 0);

            // Render
            const ImU32 col = ImGui::GetColorU32((held && isHovered) ? ImGuiCol_ButtonActive :
                isHovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

            ImVec4 imageColor = isHovered == true ? ImVec4{ 1, 1, 1, 1 } : ImVec4{ 0.5f, 0.5f, 0.5f, 1 };
            ImGui::Image((ImTextureID)texID, { 50,50 }, { 0,0 }, { 1,1 }, { 1, 1, 1, 1 }, imageColor);
        }


        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
            {
                const char* texturePath = (const char*)payload->Data;
                std::string extension = String::ExtractValue(texturePath, '.');

                auto it = textureExtensions.find("." + extension);
                if (it != textureExtensions.end())
                {
                    if (std::shared_ptr<Texture> texture = ResourceManager::GetInstance()->GetResource<Texture>(texturePath))
                    {
                        m_spheremap->m_texture = texture;
                        m_settingsChanged = true;
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::EndTable();
    }

    if (m_settingsChanged)
    {
        //true for save only
        Resource<Spheremap>::ReloadResource(m_spheremap, true);
        m_settingsChanged = false;
    }

    ContextCallback();
}

void SpheremapSettings::ContextCallback()
{
    static bool focusedNodeisHovered = false;
    if (ImGui::IsWindowHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Right) == 1)
    {
        if (isHovered)
            ImGui::OpenPopup("context");
    }

    if (ImGui::BeginPopupContextItem("context"))
    {
        ImGui::Text("Actions ...");
        ImGui::Separator();

        if (ImGui::MenuItem("Clear"))
        {
            m_spheremap->m_texture = nullptr;
            m_settingsChanged = true;
        }

        if (ImGui::MenuItem("Texture Settings"))
        {
            if (m_spheremap->m_texture)
            {
                m_assetSettingsDisplayer->SetAssetSettings(new TextureSettings(m_spheremap->m_texture));
            }
        }

        ImGui::EndPopup();
    }
}

#pragma endregion

#pragma region Cubemap
CubemapSettings::CubemapSettings(std::shared_ptr<Cubemap> cubemap)
    : m_cubemap(cubemap)
{
}

void CubemapSettings::Render()
{
    ImGui::Separator();
    ImGui::Text(std::format("Cubemap name : {}", m_cubemap->GetFilesystemPath()->filename().string()).c_str());
    ImGui::Separator();

    if (ImGui::BeginTable("texturesTable", 2))
    {
        for (int i = 0; i < m_cubemap->m_textures.size(); ++i)
        {
            ImGui::TableNextColumn();
            ImGui::Text(std::format("texture_{}", i).c_str());
            ImGui::TableNextColumn();

            uint64_t texID = 0u;
            if (m_cubemap->m_textures[i])
            {
                if (auto gpuTextureBasic = static_cast<TextureGenerator::GPUTextureBasic*>(m_cubemap->m_textures[i]->m_gpuTexture.get()))
                    texID = gpuTextureBasic->ID;
            }

            //-- Highlight --//
            {
                ImGuiContext& g = *GImGui;
                const ImGuiStyle& style = g.Style;
                ImGuiWindow* window = ImGui::GetCurrentWindow();
                const ImGuiID id = window->GetID("texturesTable");
                float cellWidth = ImGui::GetContentRegionAvail().x;
                ImVec2 pos = window->DC.CursorPos;
                ImVec2 size = ImGui::CalcItemSize({ 50, 50 }, 0, 0);

                ImVec2 posSize(pos.x + size.x, pos.y + size.y);
                ImRect bb(pos, posSize);
                ImVec2 posMin(bb.Min.x, bb.Min.y);
                ImVec2 posMax(bb.Max.x, bb.Max.y);

                bool held, hovered;
                bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

                if (hovered)
                    m_textureIdFoccused = static_cast<int>(i);

                // Render
                const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive :
                    hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

                ImVec4 imageColor = hovered == true ? ImVec4{ 1, 1, 1, 1 } : ImVec4{ 0.5f, 0.5f, 0.5f, 1 };
                ImGui::Image((ImTextureID)texID, { 50,50 }, { 0,0 }, { 1,1 }, { 1, 1, 1, 1 }, imageColor);
            }


            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
                {
                    const char* texturePath = (const char*)payload->Data;
                    std::string extension = String::ExtractValue(texturePath, '.');

                    if (textureExtensions.end() != textureExtensions.find("." + extension))
                    {
                        if (std::shared_ptr<Texture> texture = ResourceManager::GetInstance()->GetResource<Texture>(texturePath))
                        {
                            m_cubemap->m_textures[m_textureIdFoccused] = texture;
                            m_settingsChanged = true;
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
        }
        ImGui::EndTable();
    }

    if (m_settingsChanged)
    {
        //true for save only
        Resource<Cubemap>::ReloadResource(m_cubemap, true);
        m_settingsChanged = false;
    }

    ContextCallback();
}

void CubemapSettings::ContextCallback()
{
    static bool focusedNodeisHovered = false;
    if (ImGui::IsWindowHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Right) == 1)
    {
        ImGui::OpenPopup("context");
    }

    if (ImGui::BeginPopupContextItem("context"))
    {
        ImGui::Text("Actions ...");
        ImGui::Separator();

        if (ImGui::MenuItem("Clear"))
        {
            m_cubemap->m_textures[m_textureIdFoccused] = nullptr;
            m_settingsChanged = true;
        }

        if (ImGui::MenuItem("Texture Settings"))
        {
            auto texture =  m_cubemap->m_textures[m_textureIdFoccused];
            if (texture)
            {
                m_assetSettingsDisplayer->SetAssetSettings(new TextureSettings(texture));
            }
        }

        ImGui::EndPopup();
    }
}

#pragma endregion
