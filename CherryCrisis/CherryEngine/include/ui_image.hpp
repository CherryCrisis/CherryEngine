#pragma once

#include "ui_item.hpp"

#include "texture.hpp"
#include "renderer.hpp"

class ARenderingPipeline;

class UIImage : public UIItem , ARenderer
{
private:
	std::string m_texturePath = ""; // Useful for metadata 

public:
	UIImage(const char* filepath);
	std::shared_ptr<Texture> m_texture = nullptr;
	std::shared_ptr<Mesh> m_mesh = nullptr;

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
};