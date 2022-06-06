#include <pch.hpp>

#include "texture_generator.hpp"


void TextureGenerator::GPUTextureBasic::Generate(Texture* texture)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &ID);

	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, (GLint)texture->GetWrapS());
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, (GLint)texture->GetWrapT());
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, (GLint)texture->GetMinFilter());
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, (GLint)texture->GetMagFilter());

	ETextureFormat textureFormat = texture->GetInternalFormat();

	if (textureFormat == ETextureFormat::RGB || textureFormat == ETextureFormat::RGBA)
	{
		unsigned int internalFormat = GL_RGBA8;
		if (textureFormat == ETextureFormat::RGB)
			internalFormat = GL_RGB8;

		glTextureStorage2D(ID, 1, internalFormat, texture->GetWidth(), texture->GetHeight());
		glTextureSubImage2D(ID, 0, 0, 0, texture->GetWidth(), texture->GetHeight(), (GLenum)textureFormat, GL_UNSIGNED_BYTE, texture->GetData());

		glGenerateTextureMipmap(ID);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, ID);

		int mipmapsCount = texture->GetMipmapCount();
		int width = texture->GetWidth();
		int height = texture->GetHeight();
		int offset = 0;
		unsigned char* data = (unsigned char*)texture->GetData();

		for (int mipmapId = 0; mipmapId < mipmapsCount && (width || height); ++mipmapId)
		{
			CCMaths::Min(width, 1);
			CCMaths::Min(height, 1);

			int size = ((width + 3) / 4) * ((height + 3) / 4) * texture->GetBlockSize();
			glCompressedTexImage2D(GL_TEXTURE_2D, mipmapId, (GLenum)textureFormat,
				width, height, 0, size, data + offset);


			offset += size;
			width >>= 1;
			height >>= 1;
		}
	}

	//Dont clear data if is not 2D Texture to display in asset browser 
	//and after generate cubemap or spheremap with this data
	if (texture->GetSurface() == ETextureSurface::TEXTURE_2D)
		texture->ClearData();
}

void TextureGenerator::GPUTextureBasic::Regenerate(Texture* texture)
{
	Destroy();
	Generate(texture);
}

void TextureGenerator::GPUTextureBasic::Destroy()
{
	glDeleteTextures(1, &ID);
}

TextureGenerator::GPUTextureBasic::GPUTextureBasic(Texture* texture)
{
	texture->m_OnReloaded.Bind(&GPUTextureBasic::OnReload, this);

	Generate(texture);
}

TextureGenerator::GPUTextureBasic::~GPUTextureBasic()
{
	Destroy();
}

void TextureGenerator::GPUTextureBasic::OnReload(Texture* texture)
{
	Regenerate(texture);
}

bool TextureGenerator::Generate(Texture* toGenerate)
{
	if (!toGenerate)
		return false;

	if (toGenerate->m_gpuTexture2D)
		return true;

	if (!toGenerate->GetData() || toGenerate->GetWidth() <= 0 || toGenerate->GetHeight() <= 0)
		return false;

	toGenerate->m_gpuTexture2D = std::make_unique<GPUTextureBasic>(toGenerate);

	return true;
}