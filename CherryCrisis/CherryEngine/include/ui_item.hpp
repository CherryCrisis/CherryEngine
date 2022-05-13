#pragma once

#include <string>

#include "maths/vector3.hpp"
#include "object.hpp"
#include "cherry_macros.hpp"

class CCENGINE_API UIItem : public Object
{
	std::string m_name = " ";
	CCMaths::Vector3 m_position;
	CCMaths::Vector2 m_size = {50.f, 50.f};
	
protected:
	virtual void PopulateMetadatas();
public:
	UIItem() { }

	virtual void Delete();

	std::string GetName() { return m_name; }
	void		SetName(std::string name) { m_name = name; }
	CCMaths::Vector3 GetPos() { return m_position; }
	void		SetPos(const CCMaths::Vector3& pos) { m_position = pos; }

	CCMaths::Vector2 GetSize() { return m_size; }
	void		SetSize(const CCMaths::Vector2& size) { m_size = size; }


	CCProperty::CopyProperty<UIItem, std::string>		   Name{ this,     &UIItem::SetName, &UIItem::GetName };
	CCProperty::ConstRefProperty<UIItem, CCMaths::Vector3> Position{ this, &UIItem::SetPos,  &UIItem::GetPos };
	CCProperty::ConstRefProperty<UIItem, CCMaths::Vector2> Size{ this,     &UIItem::SetSize, &UIItem::GetSize};
};