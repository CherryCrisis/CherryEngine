#pragma once

#include <cherry_macros.hpp>
#include "uuid.hpp"
#include "metadata.hpp"
#include "property.hpp"

// General Object used by the engine such as entity or behaviour
class CCENGINE_API Object
{
	friend class Serializer;

private:
	CCUUID m_uuid = {};
	bool m_isActive = true;

protected:
	virtual void PopulateMetadatas() { m_metadatas.SetProperty("IsActive", &Active); };

public:
	Object(CCUUID id = {}): m_uuid(id) {};
	virtual ~Object() = default;


	Metapack m_metadatas;

	bool IsActive() { return m_isActive; }
	void SetActive(bool value) { m_isActive = value; }

	uint32_t GetUUID() const { return (uint32_t)m_uuid; }

	std::unordered_map<std::string, AMetadata*>& GetMetapack() { return m_metadatas.m_metadatas;  }

	CCProperty::CopyProperty<Object, bool> Active{ this, &Object::SetActive, &Object::IsActive };
};
