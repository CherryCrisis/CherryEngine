#include "Panels/asset_settings.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "resource_manager.hpp"

#include "material.hpp"
#include "Panels/asset_browser.hpp"
#include "texture.hpp"
#include "utils.hpp"


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

const std::map<const ETextureWrap, const int> IdOfTextureWrap
{
    {ETextureWrap::REPEAT, 0},
    {ETextureWrap::CLAMP_TO_EDGE, 1},
    {ETextureWrap::CLAMP_TO_BORDER, 2},
    {ETextureWrap::MIRRORED_REPEAT, 3},
    {ETextureWrap::MIRROR_CLAMP_TO_EDGE, 4},
};

const std::map<const int, const ETextureWrap> textureWrapByID
{
    {0, ETextureWrap::REPEAT},
    {1, ETextureWrap::CLAMP_TO_EDGE},
    {2, ETextureWrap::CLAMP_TO_BORDER},
    {3, ETextureWrap::MIRRORED_REPEAT},
    {4, ETextureWrap::MIRROR_CLAMP_TO_EDGE},
};

const std::map<const ETextureFilter, const int> IdOfTextureFilter
{
    {ETextureFilter::NEAREST, 0},
    {ETextureFilter::LINEAR, 1},
    {ETextureFilter::NEAREST_MIPMAP_NEAREST, 2},
    {ETextureFilter::NEAREST_MIPMAP_LINEAR, 3},
    {ETextureFilter::LINEAR_MIPMAP_NEAREST, 4},
    {ETextureFilter::LINEAR_MIPMAP_LINEAR, 5},
};

const std::map<const int, const ETextureFilter> textureFilterByID
{
    {0, ETextureFilter::NEAREST},
    {1, ETextureFilter::LINEAR},
    {2, ETextureFilter::NEAREST_MIPMAP_NEAREST},
    {3, ETextureFilter::NEAREST_MIPMAP_LINEAR},
    {4, ETextureFilter::LINEAR_MIPMAP_NEAREST},
    {5, ETextureFilter::LINEAR_MIPMAP_LINEAR},
};

TextureSettings::TextureSettings(std::shared_ptr<Texture> texture)
    : m_texture(texture)
{
    if (!m_texture)
        return;

    m_currentTypeId = IdOfTextureFormat.at(m_texture->GetInternalFormat());
    m_currentSurfaceId = static_cast<int>(m_texture->GetSurface());
    m_currentWrapSId = IdOfTextureWrap.at(m_texture->GetWrapS());
    m_currentWrapTId = IdOfTextureWrap.at(m_texture->GetWrapT());
    m_currentWrapRId = IdOfTextureWrap.at(m_texture->GetWrapR());
    m_currentMinFilterId = IdOfTextureFilter.at(m_texture->GetMinFilter());
    m_currentMagFilterId = IdOfTextureFilter.at(m_texture->GetMagFilter());
    m_isFlipped = m_texture->GetIsFlipped();
}

void TextureSettings::Render()
{
        ImGui::Separator();
        ImGui::Text(std::format("Texture name : {}", m_texture->GetFilesystemPath()->filename().string()).c_str());
        ImGui::Separator();

        const char* listSurface[] = { "TEXTURE_2D", "TEXTURE_CUBEMAP", "TEXTURE_SPHEREMAP"};

        if (ImGui::BeginCombo("Texture Surface", listSurface[m_currentSurfaceId]))
        {
            int textureSurfaceCount = static_cast<int>(ETextureSurface::COUNT);
            for (int n = 0; n < textureSurfaceCount; n++)
            {
                const bool is_selected = (m_currentSurfaceId == n);
                if (ImGui::Selectable(listSurface[n], is_selected))
                {
                    m_currentSurfaceId = n;
                    m_hasSettingsChanged = true;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        const char* listWrap[] = { "REPEAT", "CLAMP_TO_EDGE", "CLAMP_TO_BORDER", "MIRRORED_REPEAT", "MIRROR_CLAMP_TO_EDGE", };

        if (ImGui::BeginCombo("Texture Wrap S", listWrap[m_currentWrapSId]))
        {
            for (int n = 0; n < IdOfTextureWrap.size(); n++)
            {
                const bool is_selected = (m_currentWrapSId == n);
                if (ImGui::Selectable(listWrap[n], is_selected))
                {
                    m_currentWrapSId = n;
                    m_hasSettingsChanged = true;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Texture Wrap T", listWrap[m_currentWrapTId]))
        {
            for (int n = 0; n < IdOfTextureWrap.size(); n++)
            {
                const bool is_selected = (m_currentWrapTId == n);
                if (ImGui::Selectable(listWrap[n], is_selected))
                {
                    m_currentWrapTId = n;
                    m_hasSettingsChanged = true;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Texture Wrap R", listWrap[m_currentWrapRId]))
        {
            for (int n = 0; n < IdOfTextureWrap.size(); n++)
            {
                const bool is_selected = (m_currentWrapRId == n);
                if (ImGui::Selectable(listWrap[n], is_selected))
                {
                    m_currentWrapRId = n;
                    m_hasSettingsChanged = true;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        const char* listFilter[] = { "NEAREST", "LINEAR", "NEAREST_MIPMAP_NEAREST", "LINEAR_MIPMAP_NEAREST", "NEAREST_MIPMAP_LINEAR", "LINEAR_MIPMAP_LINEAR", };

        if (ImGui::BeginCombo("Texture Min Filter", listFilter[m_currentMinFilterId]))
        {
            for (int n = 0; n < IdOfTextureFilter.size(); n++)
            {
                const bool is_selected = (m_currentMinFilterId == n);
                if (ImGui::Selectable(listFilter[n], is_selected))
                {
                    m_currentMinFilterId = n;
                    m_hasSettingsChanged = true;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Texture Mag Filter", listFilter[m_currentMagFilterId]))
        {
            for (int n = 0; n < IdOfTextureFilter.size(); n++)
            {
                const bool is_selected = (m_currentMagFilterId == n);
                if (ImGui::Selectable(listFilter[n], is_selected))
                {
                    m_currentMagFilterId = n;
                    m_hasSettingsChanged = true;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        const char* listType[] = { "RGB", "RGBA", "DXT1", "DXT1a", "DXT3", "DXT5", "DXT6", "DXT7" };

        if (ImGui::BeginCombo("Texture Format", listType[m_currentTypeId]))
        {
            for (int n = 0; n < IdOfTextureFormat.size(); n++)
            {
                const bool is_selected = (m_currentTypeId == n);
                if (ImGui::Selectable(listType[n], is_selected))
                {
                    m_currentTypeId = n;
                    m_hasSettingsChanged = true;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        if (ImGui::Checkbox("Flip", &m_isFlipped))
            m_hasSettingsChanged = true;

        if (m_hasSettingsChanged)
        {
            if (ImGui::Button("Apply"))
            {
                m_texture->SetInternalFormat(textureFormatByID.at(m_currentTypeId));
                m_texture->SetSurface(static_cast<ETextureSurface>(m_currentSurfaceId));
                m_texture->SetWrapS(textureWrapByID.at(m_currentWrapSId));
                m_texture->SetWrapT(textureWrapByID.at(m_currentWrapTId));
                m_texture->SetWrapR(textureWrapByID.at(m_currentWrapRId));
                m_texture->SetMinFilter(textureFilterByID.at(m_currentMinFilterId));
                m_texture->SetMagFilter(textureFilterByID.at(m_currentMagFilterId));

                Resource<Texture>::ReloadResource(m_texture.get(), m_isFlipped);
                m_hasSettingsChanged = false;

                //Reaply texture surface during reload because it can be changed if the texture doesn't support cubemap settings
                m_currentSurfaceId = static_cast<int>(m_texture->GetSurface());
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
    {"CELSHADE_PALLET"},
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
                m_hasSettingsChanged = true;
                m_material->m_pipelineType = static_cast<EPipelineType>(n);
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

#pragma warning(push, 3)
#pragma warning(disable : 4804)
    if (EPipelineType::LIT == static_cast<EPipelineType>(currentPipeline))
    {
        m_hasSettingsChanged += ImGui::DragFloat3("Ambient", m_material->m_ambient.data, 0.1f, 0.f) +
            ImGui::DragFloat3("Diffuse", m_material->m_diffuse.data, 0.1f, 0.f) +
            ImGui::DragFloat3("Specular", m_material->m_specular.data, 0.1f, 0.f) +
            ImGui::DragFloat3("Emissive", m_material->m_emissive.data, 0.1f, 0.f) +
            ImGui::DragFloat("Shininess", &m_material->m_shininess, 0.1f, 0.f) +
            ImGui::Checkbox("Use cell shading", &m_material->m_hasCelshade);

    }
    else if (EPipelineType::PBR == static_cast<EPipelineType>(currentPipeline))
    {
        m_hasSettingsChanged += ImGui::DragFloat3("Albedo", m_material->m_diffuse.data, 0.1f, 0.f) +
            ImGui::DragFloat("Specular factor", &m_material->m_specularFactor, 0.1f, 0.f, 1.f) +
            ImGui::DragFloat("Metallic factor", &m_material->m_metallicFactor, 0.1f, 0.f, 1.f) +
            ImGui::DragFloat("Roughness factor", &m_material->m_roughnessFactor, 0.1f, 0.f, 1.f) +
            ImGui::DragFloat("Ambiant occlusion", &m_material->m_ao, 0.1f, 0.f, 1.f) +
            ImGui::DragFloat("Clear coat factor", &m_material->m_clearCoatFactor, 0.1f, 0.f, 1.f) +
            ImGui::DragFloat("Clear coat roughness factor", &m_material->m_clearCoatRoughnessFactor, 0.1f, 0.f, 1.f) +
            ImGui::Checkbox("Use normal map", &m_material->m_hasNormal) +
            ImGui::Checkbox("Use cell shading", &m_material->m_hasCelshade);
    }
#pragma warning(pop)

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
                if (auto gpuTextureBasic = static_cast<TextureGenerator::GPUTextureBasic*>(texRef->m_gpuTexture2D.get()))
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
                                m_hasSettingsChanged = true;
                            }
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
        }
        ImGui::EndTable();
    }

    if (m_hasSettingsChanged)
    {
        //true for save only
        Resource<Material>::ReloadResource(m_material.get(), true);
        GenerateTextureList();
        m_hasSettingsChanged = false;
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
                m_hasSettingsChanged = true;
            }
        }

        if (ImGui::MenuItem("Texture Settings"))
        {
            auto textureIt = m_textures.find(static_cast<ETextureType>(m_textureTypeFoccused));
            if (textureIt != m_textures.end())
            {
                if (textureIt->second)
                {
                    m_assetSettingsDisplayer->SetAssetSettings(std::make_unique<TextureSettings>(textureIt->second));
                }
            }
        }

        ImGui::EndPopup();
    }
}

#pragma endregion