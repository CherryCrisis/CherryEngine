#pragma once

#include <cherry_macros.hpp>

#include "texture.hpp"
#include "ui_item.hpp"

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
	std::shared_ptr<Texture> m_texture = nullptr;

	UIImage();
	UIImage(CCUUID& id);
	UIImage(const char* filepath);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;

	void Init(const std::string& str = "");
	void Delete() override;

	CCProperty::ConstRefProperty<UIImage, std::string> TexturePath{ this, &UIImage::SetTexture, &UIImage::GetTexturePath };
};