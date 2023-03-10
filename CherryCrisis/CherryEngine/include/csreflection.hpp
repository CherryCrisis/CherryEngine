#pragma once

#include "property.hpp"
#include "monowrapper.hpp"

template <typename ManagedT>
struct ReflectedProperty : public CCProperty::ConstRefProperty<ReflectedProperty<ManagedT>, ManagedT>
{
private:
	mono::ManagedProperty* m_reflectedProp = nullptr;
	mono::ManagedObject* m_csOwner = nullptr;

public:
	void SetProperty(const ManagedT& value)
	{
		m_csOwner->SetProperty(m_reflectedProp, &value);
	}

	ManagedT GetProperty()
	{
		ManagedT* valuePtr = nullptr;
		m_csOwner->GetProperty(m_reflectedProp, (void**)&valuePtr);
		return *valuePtr;
	}

	ReflectedProperty(mono::ManagedObject* owner, mono::ManagedProperty* prop)
		: CCProperty::ConstRefProperty<ReflectedProperty, ManagedT>(this, &ReflectedProperty::SetProperty, &ReflectedProperty::GetProperty), m_csOwner(owner), m_reflectedProp(prop)
	{ }
};

template <typename ManagedT>
struct ReflectedField : public CCProperty::ConstRefProperty<ReflectedField<ManagedT>, ManagedT>
{
private:
	mono::ManagedField* m_reflectedField = nullptr;
	mono::ManagedObject* m_csOwner = nullptr;

public:
	void SetField(const ManagedT& value)
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
		: CCProperty::ConstRefProperty<ReflectedField, ManagedT>(this, &ReflectedField::SetField, &ReflectedField::GetField), m_csOwner(owner), m_reflectedField(field)
	{ }
};