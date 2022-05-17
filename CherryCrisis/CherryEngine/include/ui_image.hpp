#pragma once

#include "ui_item.hpp"

#include "texture.hpp"
#include "cherry_macros.hpp"

class ARenderingPipeline;

class CCENGINE_API UIImage : public UIItem
{
private:
	std::string m_texturePath = ""; // Useful for metadata 

	void SetTexture(const std::string& path);
	std::string GetTexturePath() { return m_texturePath; }

protected:
	void PopulateMetadatas() override;

public:
	UIImage();
	UIImage(const char* filepath);
	std::shared_ptr<Texture> m_texture = nullptr;

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;

	void Init(const std::string& str = "");
	void Delete() override;

	CCProperty::ConstRefProperty<UIImage, std::string> TexturePath{ this, &UIImage::SetTexture, &UIImage::GetTexturePath };

};