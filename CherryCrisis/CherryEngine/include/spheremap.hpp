#pragma once

#include "resource.hpp"
#include "cherry_macros.hpp"

class Texture;
enum class ETextureFormat;

struct GPUSpheremap     { };
struct GPUCubemapV2     { };
struct GPUIrradianceMap { };
struct GPUPrefilterMap  { };
struct GPUBRDF          { };

class CCENGINE_API Spheremap : public Resource<Spheremap>
{
public:
    std::shared_ptr<Texture>            m_texture;
    std::unique_ptr<GPUSpheremap>       m_gpuSpheremap      = nullptr;
    std::unique_ptr<GPUCubemapV2>       m_gpuCubemapV2      = nullptr;
    std::unique_ptr<GPUIrradianceMap>   m_gpuIrradiancemap  = nullptr;
    std::unique_ptr<GPUPrefilterMap>    m_gpuPrefilterMap   = nullptr;
    std::unique_ptr<GPUBRDF>            m_gpuBrdf           = nullptr;

    Spheremap(const char* name)
        : Resource(name) { }
    ~Spheremap();

    int GetWidth();
    int GetHeight();
    ETextureFormat GetInternalFormat();

    const void* GetData();

    static void Load(std::shared_ptr<Spheremap> spheremap);

    void ClearData();
    void Delete() override;

    void Reload(bool saveOnly = false);
};
