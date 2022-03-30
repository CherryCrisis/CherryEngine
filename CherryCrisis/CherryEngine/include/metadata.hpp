#pragma once

#include "cherry_macros.hpp"

#include <any>
#include <unordered_map>
#include <typeindex>
#include <string>

#include "property.hpp"

struct CCENGINE_API Field
{
	std::string m_name;
    std::any m_value;
	std::type_index m_type = typeid(void);

	Field() = default;
	Field(const std::string& name, std::any value, const std::type_index& type)
		: m_name(name), m_value(value), m_type(type)
	{

	}
};

struct CCENGINE_API Metadata
{
	std::unordered_map<std::string, Field> m_fields;
	std::unordered_map<std::string, CCProperty::IClearProperty*> m_properties;

	template <typename CastT, typename RefT>
	void SetField(const char* fieldName, RefT& ref)
	{
		std::type_index castedTypeID = typeid(CastT);
		Field field(fieldName, std::any(std::in_place_type<CastT*>, reinterpret_cast<CastT*>(&ref)), castedTypeID);
		m_fields[fieldName] = std::move(field);
	}

	void SetProperty(const char* fieldName, CCProperty::IClearProperty* prop)
	{
		m_properties[fieldName] = { prop };
	}
};