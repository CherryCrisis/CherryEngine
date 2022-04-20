#pragma once

#include "resource.hpp"
#include "model_loader.hpp"

struct GPUTexture
{
    virtual ~GPUTexture() = default;
};

enum class ETextureFormat
{
    // https://www.reedbeta.com/blog/understanding-bcn-texture-compression-formats/

    RGB     = 0x1907,
    RGBA    = 0x1908,
    DXT1    = 0x83F0, //Compressed RGB + 1 bit alpha
    DXT1a   = 0x83F1, //Compressed RGB
    DXT3    = 0x83F2, //Compressed RGBA
    DXT5    = 0x83F3, // Compressed 2xGrayscale for tangent-space normal maps
    DXT6    = 0x8E8E, //Compressed RGB, floating point for HDR images
    DXT7    = 0x8E8C, //Compressed RGB/RGBA for High-quality color maps, color maps with full alpha

    //DXT4, //Compressed Grayscale for height maps, gloss maps any grayscale image
};

class CCENGINE_API Texture : public Resource<Texture>
{
private:
    int     m_width = 0;
    int     m_height = 0;
    int     m_size = 0;
    int     m_mipmapLevels = 0;
    int     m_internalFormat = 0;

    void*   m_data = nullptr;

public:
    std::unique_ptr<GPUTexture> m_gpuTexture = nullptr;

    Texture(const char* texturePath);
    virtual ~Texture() = default;

    int GetWidth()  { return m_width; }
    int GetHeight() { return m_height; }
    int GetSize()   { return m_size; }
    int GetMipmapCount() { return m_mipmapLevels; }
    int GetInternalFormat() { return m_internalFormat; }

    void* GetData() { return m_data; }

    static void Load(std::shared_ptr<Texture> texture, bool flipTexture = true, 
        ETextureFormat textureFormat = ETextureFormat::RGBA);

    static bool LoadFromCache(std::shared_ptr<Texture> texture, unsigned char** data, CCImporter::TextureHeader& textureHeader);
    
    void Delete() override;

    //put a texture if the texture should not be imported
    void Reload(bool flipTexture, std::shared_ptr<Texture> texture = nullptr);
};