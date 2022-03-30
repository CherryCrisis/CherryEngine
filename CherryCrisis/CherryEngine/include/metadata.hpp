#pragma once

#include "cherry_macros.hpp"

#include <any>
#include <unordered_map>
#include <string>

#include "property.hpp"

struct CCENGINE_API Field
{
	std::string m_name;
    std::any m_value;
};

struct CCENGINE_API Metadata
{
	std::unordered_map<std::string, Field> m_fields;
	std::unordered_map<std::string, CCProperty::IClearProperty*> m_properties;

	template <typename CastT, typename RefT>
	void SetField(const char* fieldName, RefT& ref)
	{
		m_fields[fieldName] = { fieldName, std::any(std::in_place_type<CastT*>, reinterpret_cast<CastT*>(&ref))};
	}

	void SetProperty(const char* fieldName, CCProperty::IClearProperty* prop)
	{
		m_properties[fieldName] = { prop };
	}
};