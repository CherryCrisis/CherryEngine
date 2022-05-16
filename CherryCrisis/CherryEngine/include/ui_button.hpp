#pragma once

#include "ui_item.hpp"
#include "cherry_macros.hpp"
#include "ui_image.hpp"
#include "ui_text.hpp"

class CCENGINE_API UIButton : public UIItem
{
private:
	//centered text
	UIText  m_text;
	//background image
	UIImage m_background;

protected:
	void PopulateMetadatas() override;
	void OnSetPos() override;
	void OnSetSize() override;
public:
	bool CompareId(int id) override;
	void Interact() override;
	UIButton();

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;


	//Managed Script : ButtonScript
	Event<> m_onClicked;
	Event<> m_onReleased;
	Event<> m_onHovered;
};