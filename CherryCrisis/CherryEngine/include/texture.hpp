#pragma once

#include "resource.hpp"
#include "model_loader.hpp"

struct GPUTexture
{
    virtual ~GPUTexture() = default;
};

class CCENGINE_API Texture : public Resource<Texture>
{
private:
    int     m_width = 0;
    int     m_height = 0;
    int     m_size = 0;

    void*   m_data = nullptr;

public:
    std::unique_ptr<GPUTexture> m_gpuTexture = nullptr;

    Texture(const char* texturePath);
    virtual ~Texture() = default;

    int GetWidth()  { return m_width; }
    int GetHeight() { return m_height; }
    int GetSize()   { return m_size; }

    void* GetData() { return m_data; }

    static void Load(std::shared_ptr<Texture> texture, bool flipTexture = true);
    static bool LoadFromCache(std::shared_ptr<Texture> texture, unsigned char** data, CCImporter::TextureHeader& textureHeader);
    
    void Delete() override;

    //put a texture if the texture should not be imported
    void Reload(bool flipTexture, std::shared_ptr<Texture> texture = nullptr);
};