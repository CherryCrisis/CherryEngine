#pragma once

#include <cherry_macros.hpp>

#include "metadata.hpp"
#include "property.hpp"
#include "uuid.hpp"

#include "event.hpp"


// General Object used by the engine such as entity or behaviour
class CCENGINE_API Object
{
	friend class Serializer;

private:
	bool	m_isActive = true;
	CCUUID	m_uuid = {};

protected:
	virtual void PopulateMetadatas() { m_metadatas.SetProperty("IsActive", &Active); };

public:
	Event<> m_OnDestroyed;

	Metapack m_metadatas;

	Object(CCUUID id = {}): m_uuid(id) {};
	virtual ~Object() { m_OnDestroyed.Invoke(); }

	bool IsActive() { return m_isActive; }
	void SetActive(bool value) { m_isActive = value; }

	uint32_t GetUUID() const { return (uint32_t)m_uuid; }

	std::unordered_map<std::string, std::shared_ptr<AMetadata>>& GetMetapack() { return m_metadatas.m_metadatas;  }

	CCProperty::CopyProperty<Object, bool> Active{ this, &Object::SetActive, &Object::IsActive };
};
