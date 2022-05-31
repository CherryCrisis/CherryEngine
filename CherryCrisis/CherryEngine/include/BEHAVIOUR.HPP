#pragma once

#include <cherry_macros.hpp>

#include "metadata.hpp"
#include "object.hpp"
#include "scene.hpp"
#include "serializer.hpp"

class Entity;

class CCENGINE_API Behaviour : public Object
{
	friend Entity;
	friend Scene;
	friend Serializer;
private:
	Entity* m_owner = nullptr;

protected:
	virtual void PopulateMetadatas() override
	{
		Object::PopulateMetadatas();
		m_metadatas.SetProperty("m_owner", &owner, "0", false);
	}

public:
	Event<> m_OnDestroy;

	Behaviour() : Object() {}
	Behaviour(CCUUID& id) : Object(id) {}
	virtual ~Behaviour() { m_OnDestroy.Invoke(); }

	virtual const std::string TypeName();
	virtual void OnSetOwner(Entity* newOwner);
	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}
	virtual void OnEnable() {}
	virtual void OnDisable() {}
	virtual void BindToSignals() {}
	virtual bool CanDelete() { return true; }
	
	virtual void Copy(Behaviour* copy) {}

	Entity& GetHost() { return *m_owner; }
	const Entity& GetHost() const { return *m_owner; }

	void	SetHostPtr(Entity* newOwner);
	Entity* GetHostPtr() { return m_owner; }

	CCProperty::CopyProperty<Behaviour, Entity*> owner{ this, &Behaviour::SetHostPtr, &Behaviour::GetHostPtr};
};
