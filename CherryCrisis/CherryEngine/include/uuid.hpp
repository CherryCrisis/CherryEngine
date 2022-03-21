#pragma once

#include <xhash>
#include <cherry_macros.hpp>


class CCENGINE_API CCUUID 
{
private:
	uint64_t m_uuid;
public:
	CCUUID();
	
	operator uint64_t() const { return m_uuid; }
};