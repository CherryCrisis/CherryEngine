#pragma once

#include "model_loader.hpp"
#include "resource.hpp"

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

    COUNT,
};

enum class CCENGINE_API ETextureWrap
{
    REPEAT = 0x2901,
    CLAMP_TO_EDGE = 0x812F,
    CLAMP_TO_BORDER = 0x812D,
    MIRRORED_REPEAT = 0x8370,
    MIRROR_CLAMP_TO_EDGE = 0x8743,
};

enum class CCENGINE_API ETextureFilter
{
    NEAREST = 0x2600,
    LINEAR = 0x2601,
    NEAREST_MIPMAP_NEAREST = 0x2700,
    NEAREST_MIPMAP_LINEAR = 0x2702,
    LINEAR_MIPMAP_NEAREST = 0x2701,
    LINEAR_MIPMAP_LINEAR = 0x2703,
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
    ETextureWrap    m_wrapS = ETextureWrap::REPEAT;
    ETextureWrap    m_wrapT = ETextureWrap::REPEAT;
    ETextureWrap    m_wrapR = ETextureWrap::REPEAT;
    ETextureFilter  m_minFilter = ETextureFilter::LINEAR;
    ETextureFilter  m_magFilter = ETextureFilter::LINEAR;
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

    int GetWidth()   const { return m_width; }
    int GetHeight()  const { return m_height; }
    int GetSize()    const { return m_size; }
    int GetMipmapCount() const { return m_mipmapLevels; }
    int GetBlockSize()  const { return m_blockSize; }

    ETextureFormat GetInternalFormat() const { return m_internalFormat; }
    void SetInternalFormat(ETextureFormat textureFormat) { m_internalFormat = textureFormat; }

    ETextureSurface GetSurface()  const { return m_surface; }
    void SetSurface(ETextureSurface surface) { m_surface = surface; }

    ETextureWrap GetWrapS() const { return m_wrapS; }
    void SetWrapS(ETextureWrap wrap) { m_wrapS = wrap; }

    ETextureWrap GetWrapT()  const { return m_wrapT; }
    void SetWrapT(ETextureWrap wrap) { m_wrapT = wrap; }

    ETextureWrap GetWrapR()  const { return m_wrapR; }
    void SetWrapR(ETextureWrap wrap) { m_wrapR = wrap; }

    ETextureFilter GetMinFilter() const { return m_minFilter; }
    void SetMinFilter(ETextureFilter filter) { m_minFilter = filter; }

    ETextureFilter GetMagFilter()  const { return m_magFilter; }
    void SetMagFilter(ETextureFilter filter) { m_magFilter = filter; }

    bool GetIsFlipped() const { return m_flipped; }

    void* GetData() { return m_data; }
    void ClearData();

    static void Load(std::shared_ptr<Texture> texture, void* data, int width = 1, int height = 1,
        ETextureFormat textureFormat = ETextureFormat::RGBA, ETextureFilter textureMinFilter = ETextureFilter::LINEAR, ETextureFilter textureMagFilter = ETextureFilter::LINEAR, ETextureWrap textureWrapS = ETextureWrap::REPEAT, ETextureWrap textureWrapT = ETextureWrap::REPEAT, ETextureWrap textureWrapR = ETextureWrap::REPEAT);

    static void Load(std::shared_ptr<Texture> texture, bool flipTexture = true, 
        ETextureFormat textureFormat = ETextureFormat::DXT1, ETextureFilter textureMinFilter = ETextureFilter::LINEAR, ETextureFilter textureMagFilter = ETextureFilter::LINEAR, ETextureWrap textureWrapS = ETextureWrap::REPEAT, ETextureWrap textureWrapT = ETextureWrap::REPEAT, ETextureWrap textureWrapR = ETextureWrap::REPEAT);

    static bool LoadFromCache(Texture* texture, unsigned char** data, CCImporter::TextureHeader& textureHeader);
    
    void Delete() override;

    void Reload(bool flipTexture = true, bool importTexture = true);
};