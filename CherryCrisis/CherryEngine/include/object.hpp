#pragma once

#include <cherry_macros.hpp>
#include "uuid.hpp"
#include "metadata.hpp"

// General Object used by the engine such as entity or behaviour
class CCENGINE_API Object
{
	friend class Serializer;

private:
	CCUUID m_uuid = {};

public:
	Object(CCUUID id = {}): m_uuid(id) {};
	virtual ~Object() = default;

	uint32_t GetUUID() const { return (uint32_t)m_uuid; }

	Metapack m_metadatas;
};