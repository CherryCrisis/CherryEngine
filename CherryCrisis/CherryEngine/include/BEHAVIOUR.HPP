#pragma once

#include <cherry_macros.hpp>

#include "object.hpp"
#include "entity.hpp"

#include "metadata.hpp"
#include "scene.hpp"
#include "serializer.hpp"

class CCENGINE_API Behaviour : public Object
{
	friend class Entity;
	friend class Scene;
	friend class Serializer;
private:
	Entity* m_owner = nullptr;
protected:

	virtual void PopulateMetadatas() override
	{
		Object::PopulateMetadatas();
		m_metadatas.SetProperty("m_owner", &owner);
	}

public:
	Event<> m_OnDestroy;

	Behaviour() = default;
	Behaviour(CCUUID& id) : Object(id) {}
	virtual ~Behaviour() { m_OnDestroy.Invoke(); }

	virtual void OnSetOwner(Entity* newOwner);
	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}
	virtual void OnEnable() {}
	virtual void OnDisable() {}
	virtual void BindToSignals() {}
	
	virtual void Copy(Behaviour* copy) {}

	Entity& GetHost() { return *m_owner; }
	const Entity& GetHost() const { return *m_owner; }

	Entity* GetHostPtr() { return m_owner; }
	void SetHostPtr(Entity* newOwner);
	CCProperty::CopyProperty<Behaviour, Entity*> owner{ this, &Behaviour::SetHostPtr, &Behaviour::GetHostPtr};
};
