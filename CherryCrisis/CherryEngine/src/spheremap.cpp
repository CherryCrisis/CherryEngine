#include "pch.hpp"

#include "spheremap.hpp"

#include "resource_manager.hpp"

#include "texture.hpp"

Spheremap::~Spheremap()
{
    if (GetResourceState() != EResourceState::DESTROYED)
        Delete();
}


void Spheremap::Load(std::shared_ptr<Spheremap> spheremap, const char* texturePath)
{
    ResourceManager* resourceManager = ResourceManager::GetInstance();
    spheremap->m_texture = ResourceManager::GetInstance()->AddResource<Texture>(texturePath, true, true, ETextureFormat::RGB);
}

int Spheremap::GetWidth()
{
    return m_texture->GetWidth();
}

int Spheremap::GetHeight()
{
    return m_texture->GetHeight();
}

ETextureFormat Spheremap::GetInternalFormat()
{
    return m_texture->GetInternalFormat();
}

const void* Spheremap::GetData()
{
    return m_texture->GetData();
}

void Spheremap::ClearData()
{
    m_texture->ClearData();
}
void Spheremap::Delete()
{
    ClearData();
}
