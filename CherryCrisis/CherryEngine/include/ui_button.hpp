#pragma once

#include "ui_item.hpp"
#include "cherry_macros.hpp"
#include "ui_image.hpp"
#include "ui_text.hpp"
#include "ui_button_behaviour.hpp"

class CCENGINE_API UIButton : public UIItem
{
private:
	//centered text
	UIText  m_text;
	//background image
	UIImage m_background;

	ButtonBehaviour behaviour;
	std::string behaviourPath = " ";
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

	void SetBehaviourName(const std::string& path);

	std::string GetBehaviourName() {return behaviourPath;}

	//Managed Script : ButtonScript
	Event<> m_onClicked;
	Event<> m_onReleased;
	Event<> m_onHovered;

	CCProperty::ConstRefProperty < UIButton, std::string > Behaviour{ this, &UIButton::SetBehaviourName, &UIButton::GetBehaviourName };
};