#pragma once

#include "property.hpp"
#include "monowrapper.hpp"

template <typename ManagedT>
struct ReflectedProperty : public CCProperty::Property<ReflectedProperty<ManagedT>, ManagedT>
{
private:
	mono::ManagedProperty* m_reflectedField = nullptr;
	mono::ManagedObject* m_csOwner = nullptr;

public:
	void SetProperty(const ManagedT& value)
	{
		m_csOwner->GetProperty(m_reflectedField, &value);
	}

	ManagedT GetProperty()
	{
		ManagedT value;
		m_csOwner->GetProperty(m_reflectedField, &value);
		return value;
	}

	ReflectedProperty(mono::ManagedObject* owner, mono::ManagedProperty* prop)
		: CCProperty::Property<ReflectedProperty, ManagedT>(this, &ReflectedProperty::SetProperty, &ReflectedProperty::GetProperty), m_csOwner(owner), m_reflectedField(prop)
	{ }
};

template <typename ManagedT>
struct ReflectedField : public CCProperty::Property<ReflectedField<ManagedT>, ManagedT>
{
private:
	mono::ManagedField* m_reflectedField = nullptr;
	mono::ManagedObject* m_csOwner = nullptr;

public:
	void SetField(ManagedT& value)
	{
		m_csOwner->SetField(m_reflectedField, &value);
	}

	ManagedT GetField()
	{
		ManagedT value;
		m_csOwner->GetField(m_reflectedField, &value);
		return value;
	}

	ReflectedField(mono::ManagedObject* owner, mono::ManagedField* field)
		: CCProperty::Property<ReflectedField, ManagedT>(this, &ReflectedField::SetField, &ReflectedField::GetField), m_csOwner(owner), m_reflectedField(field)
	{ }
};