#pragma once

#include "cherry_macros.hpp"

#include <any>
#include <typeindex>
#include <string>

struct CCENGINE_API Field
{
	std::any m_value;
	std::type_index m_type = typeid(void);

	Field(std::any value, const std::type_index& type)
		: m_value(value), m_type(type)
	{

	}

	virtual ~Field() = default;
};