#pragma once

#include <cherry_macros.hpp>

#include "ui_button_behaviour.hpp"
#include "ui_image.hpp"
#include "ui_item.hpp"
#include "ui_text.hpp"


class CCENGINE_API UIButton : public UIItem
{
private:
	//centered text
	UIText  m_text;
	//background image
	UIImage m_background;

	ButtonBehaviour behaviour;
	std::string behaviourPath = " ";

	void Init();
protected:
	void PopulateMetadatas() override;
	void OnSetPos() override;
	void OnSetSize() override;

public:
	Event<> m_onClicked;
	Event<> m_onReleased;
	Event<> m_onHovered;
	
	UIButton(CCUUID& id);
	UIButton();
	void Delete() override;
	bool CompareId(int id) override;
	void Interact() override;
	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;

	bool IsHovered() override;
	void SetHovered(bool value) override;
	void SetBehaviourName(const std::string& path);

	std::string GetBehaviourName() {return behaviourPath;}

	CCProperty::ConstRefProperty<UIButton, std::string> Behaviour{ this, &UIButton::SetBehaviourName, &UIButton::GetBehaviourName };
};