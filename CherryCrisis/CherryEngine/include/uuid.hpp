#pragma once

#include <xhash>
#include <cherry_macros.hpp>


class CCENGINE_API CCUUID 
{
private:
	uint64_t m_uuid;
public:
	CCUUID();
	CCUUID(uint64_t uuid): m_uuid(uuid) {}
	CCUUID(const CCUUID& uuid) { m_uuid = uuid.m_uuid; }

	operator uint64_t() const { return m_uuid; }
};

namespace std 
{
	template<>
	struct hash<CCUUID> 
	{
		std::size_t operator()(const CCUUID& ref) const 
		{
			return hash<uint64_t>()((uint64_t)ref);
		}
	};
}