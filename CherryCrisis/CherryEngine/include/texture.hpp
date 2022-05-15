#pragma once

#include "resource.hpp"
#include "model_loader.hpp"

struct CCENGINE_API GPUTexture2D
{
    virtual ~GPUTexture2D() = default;
};

struct CCENGINE_API GPUTextureSpheremap
{
    virtual ~GPUTextureSpheremap() = default;
};

struct CCENGINE_API GPUTextureCubemap
{
    virtual ~GPUTextureCubemap() = default;
};

struct CCENGINE_API GPUIrradianceMap
{
    virtual ~GPUIrradianceMap() = default;
};

struct CCENGINE_API GPUPrefilterMap
{
    virtual ~GPUPrefilterMap() = default;
};

enum class CCENGINE_API ETextureSurface
{
    TEXTURE_2D,
    TEXTURE_CUBEMAP,
    TEXTURE_SPHEREMAP,
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
    ETextureSurface m_surface = ETextureSurface::TEXTURE_2D;
    bool            m_flipped = false;

    void*           m_data = nullptr;

    bool m_stackAllocated = false;

public:
    std::unique_ptr<GPUTexture2D> m_gpuTexture2D = nullptr;
    std::unique_ptr<GPUTextureSpheremap> m_gpuTextureSpheremap = nullptr;
    std::unique_ptr<GPUTextureCubemap> m_gpuTextureCubemap = nullptr;

    //-- PBR --//
    std::unique_ptr<GPUIrradianceMap>   m_gpuIrradiancemap = nullptr;
    std::unique_ptr<GPUPrefilterMap>    m_gpuPrefilterMap = nullptr;
    
    Texture(const char* texturePath);
    virtual ~Texture();

    int GetWidth()  { return m_width; }
    int GetHeight() { return m_height; }
    int GetSize()   { return m_size; }
    int GetMipmapCount() { return m_mipmapLevels; }
    int GetBlockSize() { return m_blockSize; }

    ETextureFormat GetInternalFormat() { return m_internalFormat; }
    void SetInternalFormat(ETextureFormat textureFormat) { m_internalFormat = textureFormat; }

    ETextureSurface GetSurface() { return m_surface; }
    void SetSurface(ETextureSurface surface) { m_surface = surface; }


    bool GetIsFlipped() const { return m_flipped; }

    void* GetData() { return m_data; }
    void ClearData();

    static void Load(std::shared_ptr<Texture> texture, void* data, int width = 1, int height = 1,
        ETextureFormat textureFormat = ETextureFormat::RGBA);

    static void Load(std::shared_ptr<Texture> texture, bool flipTexture = true, 
        ETextureFormat textureFormat = ETextureFormat::DXT1);

    static bool LoadFromCache(std::shared_ptr<Texture> texture, unsigned char** data, CCImporter::TextureHeader& textureHeader);
    
    void Delete() override;

    void Reload(bool flipTexture = true);
};