#pragma once

#include "ui_item.hpp"

#include "font.hpp"

class UIText : public UIItem
{
private:
	std::string m_value	   = "sample text";
	std::string m_fontPath = " ";
	float		m_size	   = 50.f;
protected: 
	void PopulateMetadatas() override;

public:
	std::shared_ptr<Font> m_font;
	
	UIText();
	UIText(CCUUID& id);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;

	void SetFont(const std::string& path);
	void SetText(const std::string& value) {m_value = value;}
	std::string GetFontPath() { return m_fontPath; }
	std::string GetText() { return m_value; }
	void SetSize(const float& value) { m_size = value; }
	float GetSize() { return m_size; }

	CCProperty::ConstRefProperty<UIText, std::string> FontPath{ this, &UIText::SetFont, &UIText::GetFontPath };
	CCProperty::ConstRefProperty<UIText, std::string> Text{ this, &UIText::SetText, &UIText::GetText };
	CCProperty::ConstRefProperty<UIText, float> Size{ this, &UIText::SetSize, &UIText::GetSize };
};