#pragma once

#include <xhash>
#include <cherry_macros.hpp>


class CCENGINE_API CCUUID 
{
private:
	uint32_t m_uuid;
public:
	CCUUID();
	CCUUID(uint32_t uuid): m_uuid(uuid) {}
	CCUUID(const CCUUID& uuid) { m_uuid = uuid.m_uuid; }

	operator uint32_t() const { return m_uuid; }
};

namespace std 
{
	template<>
	struct hash<CCUUID> 
	{
		std::size_t operator()(const CCUUID& ref) const 
		{
			return hash<uint32_t>()((uint32_t)ref);
		}
	};
}