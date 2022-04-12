#pragma once

#include "resource.hpp"
#include "model_loader.hpp"
//namespace CCModelLoader
//{
//    struct HeaderTexture;
//}

struct GPUTexture { };

class CCENGINE_API Texture : public Resource<Texture>
{
private:

    int     m_width = 0;
    int     m_height = 0;

    void*   m_data = nullptr;

    std::string m_relativePath;

public:
    Texture(const char* texturePath);

    GPUTexture* m_gpuTexture = nullptr;

    ~Texture() = default;

    int GetWidth()  { return m_width; }
    int GetHeight() { return m_height; }

    void* GetData() { return m_data; }

    //static void Load(std::shared_ptr<Texture> texture, bool flipTexture);
    static void Load(std::shared_ptr<Texture> texture, bool flipTexture = true, const char* relativePath = "");
    static bool LoadFromCache(std::shared_ptr<Texture> texture, unsigned char** data, CCImporter::TextureHeader& textureHeader);
    
    void Delete() override;
    void Reload();
};