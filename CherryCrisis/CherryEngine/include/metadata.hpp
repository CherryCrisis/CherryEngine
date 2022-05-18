#pragma once

#include "cherry_macros.hpp"

#include <any>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <string>

#include "property.hpp"
#include "field.hpp"

struct CCENGINE_API AMetadata
{
	virtual void Get(void** outValue) = 0;
	virtual void Set(void* inValue) = 0;

	virtual const std::type_index& GetType() = 0;

	std::string m_identifier;
	std::string m_name;

	AMetadata(const char* name, const char* identifier)
		: m_identifier(identifier) { }
};

template <typename T>
struct CCENGINE_API MetaField : public AMetadata
{
	Field m_field;

	void Get(void** outValue)
	{
		*outValue = std::any_cast<T>(m_field.m_value);
	}

	void Set(void* inValue)
	{
		*std::any_cast<T>(m_field.m_value) = *reinterpret_cast<T>(inValue);
	}

	const std::type_index& GetType() override
	{
		return m_field.m_type;
	}

	MetaField(const char* name, const Field& field, const char* identifier)
		: AMetadata(name, identifier), m_field(field)
	{

	}

	MetaField(const char* name, std::any value, const std::type_index& type, const char* identifier)
		: AMetadata(name, identifier), m_field(value, type)
	{

	}
};

struct CCENGINE_API MetaProperty : public AMetadata
{
	CCProperty::IClearProperty* m_property = nullptr;

	void Get(void** outValue) override
	{
		m_property->Get(*outValue);
	}

	void Set(void* inValue) override
	{
		m_property->Set(inValue);
	}

	const std::type_index& GetType() override
	{
		return m_property->GetGetType();
	}

	MetaProperty(const char* name, CCProperty::IClearProperty* prop, const char* identifier)
		: AMetadata(name, identifier), m_property(prop)
	{

	}
};

struct CCENGINE_API Metapack
{
	std::unordered_map<std::string, std::shared_ptr<AMetadata>> m_metadatas;

	template <typename T>
	void SetField(const char* fieldName, Field& fieldToSet, const char* identifier = "0")
	{
		// todo use unique ptr
		m_metadatas[fieldName] = std::make_shared<MetaField<T>>(fieldName, fieldToSet, identifier);
	}

	template <typename T>
	void SetField(const char* fieldName, std::any value, const std::type_index& fieldType, const char* identifier = "0")
	{
		// todo use unique ptr

		m_metadatas[fieldName] = std::make_shared<MetaField<T>>(fieldName, value, fieldType, identifier);
	}

	template <typename CastT, typename RefT>
	void SetFieldFromPtr(const char* fieldName, RefT* ref)
	{
		SetField<CastT*>(fieldName, std::any(std::in_place_type<CastT>, reinterpret_cast<CastT>(ref)), typeid(std::remove_pointer<CastT>));
	}

	template <typename RefT>
	void SetFieldFromPtr(const char* fieldName, RefT* ref)
	{
		SetField<RefT*>(fieldName, std::any(ref), typeid(RefT));
	}

	template <typename RefT>
	void SetField(const char* fieldName, RefT& ref)
	{
		SetFieldFromPtr(fieldName, &ref);
	}

	template <typename CastT, typename RefT>
	void SetField(const char* fieldName, RefT& ref)
	{
		SetField<CastT*>(fieldName, std::any(std::in_place_type<CastT*>, reinterpret_cast<CastT*>(&ref)), typeid(CastT));
	}

	void SetProperty(const char* fieldName, CCProperty::IClearProperty* prop, const char* identifier = "0")
	{
		// todo use unique ptr

		m_metadatas[fieldName] = std::make_shared<MetaProperty>(fieldName, prop, identifier);
	}
};