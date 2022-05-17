#pragma once

#include "cherry_macros.hpp"

#include <any>
#include <unordered_map>
#include <typeindex>
#include <string>

#include "property.hpp"
#include "field.hpp"

struct CCENGINE_API Metadata
{
};


struct CCENGINE_API Metapack
{
	std::unordered_map<std::string, Field> m_fields;
	std::unordered_map<std::string, CCProperty::IClearProperty*> m_properties;

	void SetField(const char* fieldName, Field& fieldToSet)
	{
		m_fields[fieldName] = std::move(fieldToSet);
	}

	void SetField(const char* fieldName, std::any value, std::type_index fieldType)
	{
		m_fields[fieldName] = Field(fieldName, value, fieldType);
	}

	template <typename CastT, typename RefT>
	void SetFieldFromPtr(const char* fieldName, RefT* ref)
	{
		SetField(fieldName, std::any(std::in_place_type<CastT>, reinterpret_cast<CastT>(ref)), typeid(std::remove_pointer<CastT>));
	}

	template <typename RefT>
	void SetFieldFromPtr(const char* fieldName, RefT* ref)
	{
		SetField(fieldName, std::any(ref), typeid(RefT));
	}

	template <typename RefT>
	void SetField(const char* fieldName, RefT& ref)
	{
		SetFieldFromPtr(fieldName, &ref);
	}

	template <typename CastT, typename RefT>
	void SetField(const char* fieldName, RefT& ref)
	{
		SetField(fieldName, std::any(std::in_place_type<CastT*>, reinterpret_cast<CastT*>(&ref)), typeid(CastT));
	}

	void SetProperty(const char* fieldName, CCProperty::IClearProperty* prop)
	{
		m_properties[fieldName] = { prop };
	}
};