#include "Panels/assets_settings.hpp"

#include <imgui.h>

#include "resource_manager.hpp"

#include "texture.hpp"

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
    m_isFlipped = m_texture->IsFlipped();
}

bool TextureSettings::Update()
{
    ImGui::OpenPopup(m_popupName);

    bool isOpen;

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(m_popupName, &isOpen, ImGuiWindowFlags_AlwaysAutoResize))
    {
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
        
        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            return false;
        }

        ImGui::SameLine();

        if (m_settingsChanged)
        {
            if (ImGui::Button("Save"))
            {
                m_texture->SetInternalFormat(textureFormatByID.at(m_currentId));
                Resource<Texture>::ReloadResource(m_texture, m_isFlipped);
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                return false;
            }
        }

        ImGui::EndPopup();
    }

    return isOpen;
}
