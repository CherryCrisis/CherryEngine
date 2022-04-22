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

    RGB     = 0x80E0,
    RGBA    = 0x80E1,
    DXT1    = 0x83F0, //Compressed RGB
    DXT1a   = 0x83F1, //Compressed RGB + 1 bit alpha
    DXT3    = 0x83F2, //Compressed RGBA
    DXT5    = 0x83F3, // Compressed 2xGrayscale for tangent-space normal maps
    DXT6    = 0x8E8E, //Compressed RGB, floating point for HDR images
    DXT7    = 0x8E8C, //Compressed RGB/RGBA for High-quality color maps, color maps with full alpha

    //DXT4, //Compressed Grayscale for height maps, gloss maps any grayscale image
};



class CCENGINE_API Texture : public Resource<Texture>
{
private:
    int             m_width = 0;
    int             m_height = 0;
    int             m_size = 0;
    int             m_mipmapLevels = 0;
    int             m_blockSize = 0;
    ETextureFormat  m_internalFormat = ETextureFormat::RGBA;

    void*   m_data = nullptr;

public:
    std::unique_ptr<GPUTexture> m_gpuTexture = nullptr;

    Texture(const char* texturePath);
    virtual ~Texture() = default;

    int GetWidth()  { return m_width; }
    int GetHeight() { return m_height; }
    int GetSize()   { return m_size; }
    int GetMipmapCount() { return m_mipmapLevels; }
    int GetBlockSize() { return m_blockSize; }
    ETextureFormat GetInternalFormat() { return m_internalFormat; }
    void SetInternalFormat(ETextureFormat textureFormat) { m_internalFormat = textureFormat; }

    void* GetData() { return m_data; }

    static void Load(std::shared_ptr<Texture> texture, bool flipTexture = true, 
        ETextureFormat textureFormat = ETextureFormat::RGBA);

    static bool LoadFromCache(std::shared_ptr<Texture> texture, unsigned char** data, CCImporter::TextureHeader& textureHeader);
    
    void Delete() override;

    //put a texture if the texture should not be imported
    void Reload(bool flipTexture, std::shared_ptr<Texture> texture = nullptr);
};