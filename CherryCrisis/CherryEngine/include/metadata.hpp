#pragma once

#include <any>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include <cherry_macros.hpp>

#include "field.hpp"
#include "property.hpp"


struct CCENGINE_API AMetadata
{
	std::string m_identifier;
	std::string m_name;
	bool m_isShownOnInspector = true;

	AMetadata(const char* name, const char* identifier, bool shownOnInspector = true)
		: m_identifier(identifier), m_isShownOnInspector(shownOnInspector) { }
		
	virtual void Get(void** outValue) = 0;
	virtual void Set(void* inValue) = 0;

	virtual const std::type_index& GetType() = 0;
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

	MetaField(const char* name, const Field& field, const char* identifier, bool shown)
		: AMetadata(name, identifier, shown), m_field(field)
	{

	}

	MetaField(const char* name, std::any value, const std::type_index& type, const char* identifier, bool shown)
		: AMetadata(name, identifier, shown), m_field(value, type)
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

	MetaProperty(const char* name, CCProperty::IClearProperty* prop, const char* identifier, bool shown=true)
		: AMetadata(name, identifier, shown), m_property(prop)
	{

	}
};

struct CCENGINE_API Metapack
{
	std::vector<std::string> m_insertOrder;
	std::unordered_map<std::string, std::shared_ptr<AMetadata>> m_metadatas;

	void SetMetadata(const char* fieldName, std::shared_ptr<AMetadata> metadata)
	{
		m_insertOrder.push_back(fieldName);
		m_metadatas.insert({ fieldName, metadata });
	}

	template <typename T>
	void SetField(const char* fieldName, Field& fieldToSet, bool shown = true, const char* identifier = "\0")
	{
		SetMetadata(fieldName, std::make_shared<MetaField<T>>(fieldName, fieldToSet, identifier, shown));
	}

	template <typename T>
	void SetField(const char* fieldName, std::any value, const std::type_index& fieldType, bool shown = true, const char* identifier = "\0")
	{
		SetMetadata(fieldName, std::make_shared<MetaField<T>>(fieldName, value, fieldType, identifier, shown));
	}

	template <typename CastT, typename RefT>
	void SetFieldFromPtr(const char* fieldName, RefT* ref, bool shown = true, const char* identifier = "\0")
	{
		SetField<CastT*>(fieldName, std::any(std::in_place_type<CastT>, reinterpret_cast<CastT>(ref)), typeid(std::remove_pointer<CastT>), shown, identifier);
	}

	template <typename RefT>
	void SetFieldFromPtr(const char* fieldName, RefT* ref, bool shown = true, const char* identifier = "\0")
	{
		SetField<RefT*>(fieldName, std::any(ref), typeid(RefT), shown, identifier);
	}

	template <typename RefT>
	void SetField(const char* fieldName, RefT& ref, bool shown = true, const char* identifier = "\0")
	{
		SetFieldFromPtr(fieldName, &ref, shown, identifier);
	}

	template <typename CastT, typename RefT>
	void SetField(const char* fieldName, RefT& ref, bool shown = true, const char* identifier = "\0")
	{
		SetField<CastT*>(fieldName, std::any(std::in_place_type<CastT*>, reinterpret_cast<CastT*>(&ref)), typeid(CastT), shown, identifier);
	}

	void SetProperty(const char* fieldName, CCProperty::IClearProperty* prop, const char* identifier = "\0", bool shown = true)
	{
		SetMetadata(fieldName, std::make_shared<MetaProperty>(fieldName, prop, identifier, shown));
	}
};
