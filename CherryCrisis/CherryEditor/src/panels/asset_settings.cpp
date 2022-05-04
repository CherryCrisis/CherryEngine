#include "Panels/asset_settings.hpp"

#include <imgui.h>

#include "resource_manager.hpp"

#include "texture.hpp"
#include "material.hpp"
#include "utils.hpp"
#include "basic_renderpass.hpp"
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
            m_textures.emplace(pair->first, pair->second);
        else
            m_textures.emplace(currentTypeID, nullptr);
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
            ImGui::DragFloat("Clear coat roughness factor", &m_material->m_clearCoatRoughnessFactor, 0.1f, 0.f, 1.f);
    }

    if (ImGui::BeginTable("texturesTable", 2))
    {
        for (auto& [texType, texRef] : m_textures)
        {
            ImGui::TableNextColumn();
            ImGui::Text(TextureTypeToStr[static_cast<int>(texType)].c_str());

            ImGui::TableNextColumn();

            BasicRenderPass::GPUTextureBasic* GPUtex = nullptr;
            if (texRef.get() && texRef->m_gpuTexture)
            {
                GPUtex = static_cast<BasicRenderPass::GPUTextureBasic*>
                    (texRef->m_gpuTexture.get());
            }

            uint64_t texID = GPUtex ? (uint64_t)GPUtex->ID : 0u;

            ImGui::ImageButton((ImTextureID)texID, { 50,50 });

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
                {
                    const char* texturePath = (const char*)payload->Data;
                    std::string extension = String::ExtractValue(texturePath, '.');

                    auto it = textureExtensions.find("." + extension);
                    if (it != textureExtensions.end())
                    {
                        auto textureIt = m_material->m_textures.find(texType);
                        if (textureIt != m_material->m_textures.end())
                        {
                            if (std::shared_ptr<Texture> texture = ResourceManager::GetInstance()->GetResource<Texture>(texturePath))
                            {
                                textureIt->second = texture;
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

}

#pragma endregion