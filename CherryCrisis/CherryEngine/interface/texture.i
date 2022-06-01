%include "enums.swg"

%{
	#include "texture.hpp"
%}

enum class ETextureSurface
{
    TEXTURE_2D,
    TEXTURE_CUBEMAP,
    TEXTURE_SPHEREMAP,

    COUNT,
};

enum class ETextureFormat
{
    RGB     = 0x80E0,
    RGBA    = 0x80E1,
    DXT1    = 0x83F0, //Compressed RGB
    DXT1a   = 0x83F1, //Compressed RGB + 1 bit alpha
    DXT3    = 0x83F2, //Compressed RGBA
    DXT5    = 0x83F3, // Compressed 2xGrayscale for tangent-space normal maps
    DXT6    = 0x8E8E, //Compressed RGB, floating point for HDR images
    DXT7    = 0x8E8C, //Compressed RGB/RGBA for High-quality color maps, color maps with full alpha
};

%nodefaultctor Texture;

%include <std_shared_ptr.i>
%shared_ptr(Texture)

class Texture
{
public:
    int GetWidth();
    int GetHeight();
    int GetSize();
    int GetMipmapCount();
    int GetBlockSize();

    ETextureFormat GetInternalFormat();
    void SetInternalFormat(ETextureFormat textureFormat);

    ETextureSurface GetSurface();
    void SetSurface(ETextureSurface surface);

    bool GetIsFlipped() const;
};