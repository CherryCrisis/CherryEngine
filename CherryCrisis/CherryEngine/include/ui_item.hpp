#pragma once

#include <memory>
#include <string>

#include <cherry_macros.hpp>

#include "maths/vector3.hpp"
#include "maths/matrix4.hpp"
#include "object.hpp"
#include "renderer.hpp"

class Mesh;


class CCENGINE_API UIItem : public Object, public ARenderer
{
private:
	std::string m_name = " ";
	CCMaths::Vector2 m_size = {50.f, 50.f};
	CCMaths::Matrix4 m_model = CCMaths::Matrix4::Identity;
	
	static int count;

protected:
	virtual void PopulateMetadatas();
	virtual void OnSetPos() {};
	virtual void OnSetSize() {};

public:
	int m_id = -1;

	CCMaths::Vector3 m_position;
	std::shared_ptr<Mesh> m_mesh = nullptr;
	
	UIItem();
	UIItem(CCUUID& id);

	virtual bool CompareId(int id) { return id == m_id; };
	virtual void Delete();
	virtual void Interact() {};
	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;

	std::string GetName() { return m_name; }
	void		SetName(std::string name) { m_name = name; }
	CCMaths::Vector3 GetPos() { return m_position; }
	void		SetPos(const CCMaths::Vector3& pos) { m_position = pos; OnSetPos(); }

	CCMaths::Vector2 GetSize() { return m_size; }
	void		SetSize(const CCMaths::Vector2& size) { m_size = size; OnSetSize(); }

	// TODO: Add dirty flag
	CCMaths::Matrix4 GetModel() { return CCMaths::Matrix4::Translate(m_position) * CCMaths::Matrix4::Scale({ m_size, 1.f }); }

	CCProperty::CopyProperty<UIItem, std::string>		   Name{ this,     &UIItem::SetName, &UIItem::GetName };
	CCProperty::ConstRefProperty<UIItem, CCMaths::Vector3> Position{ this, &UIItem::SetPos,  &UIItem::GetPos };
	CCProperty::ConstRefProperty<UIItem, CCMaths::Vector2> Size{ this,     &UIItem::SetSize, &UIItem::GetSize};
};