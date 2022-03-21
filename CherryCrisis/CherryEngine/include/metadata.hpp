#pragma once

#include "cherry_macros.hpp"

#include <any>
#include <vector>
#include <string>

enum class CCENGINE_API DescriptorType
{
    UNDEFINED,
    INT,
    FLOAT,
    PTR,
    VECTOR3,
};

struct CCENGINE_API Field
{
	std::string m_name;
    std::any m_value;
};

struct CCENGINE_API Metadata
{
	std::vector<Field> m_fields;
};