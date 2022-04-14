#pragma once

#include "resource.hpp"
#include "model_loader.hpp"
//namespace CCModelLoader
//{
//    struct HeaderTexture;
//}

struct GPUTexture
{
    virtual ~GPUTexture() = default;
};

class CCENGINE_API Texture : public Resource<Texture>
{
private:
    int     m_width = 0;
    int     m_height = 0;

    void*   m_data = nullptr;


public:
    std::unique_ptr<GPUTexture> m_gpuTexture = nullptr;

    Texture(const char* texturePath);
    virtual ~Texture() = default;

    int GetWidth()  { return m_width; }
    int GetHeight() { return m_height; }

    void* GetData() { return m_data; }

    static void Load(std::shared_ptr<Texture> texture, bool flipTexture);
    static void Load(std::shared_ptr<Texture> texture);
    static bool LoadFromCache(std::shared_ptr<Texture> texture, unsigned char** data, CCImporter::TextureHeader& textureHeader);
    
    void Delete() override;
    void Reload();
};