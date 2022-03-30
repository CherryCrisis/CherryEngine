#pragma once

#include "csreflection.hpp"

#include <locale>
#include <codecvt>
#include <cstdlib>

template <>
struct ReflectedField<std::string> : public CCProperty::Property<ReflectedField<std::string>, std::string>
{
private:
	mono::ManagedField* m_reflectedField = nullptr;
	mono::ManagedObject* m_csOwner = nullptr;

public:
	void SetField(std::string& value)
	{
		size_t wideStrLen = 0u;
		mbstate_t state {};

		const char* cs = value.c_str();
		std::unique_ptr<wchar_t[]> buf(new wchar_t[value.size()]());

		mbsrtowcs_s(&wideStrLen, buf.get(), value.size(), &cs, 50000, &state);

		MonoString* monoString = mono_string_from_utf16(buf.get());
		m_csOwner->SetField(m_reflectedField, monoString);
	}

	std::string GetField()
	{
		MonoString* managedValue;
		m_csOwner->GetField(m_reflectedField, &managedValue);

		char* monoChar = mono_string_to_utf8(managedValue);
		std::string value = monoChar;
		mono_free(monoChar);
		return value;
	}

	ReflectedField(mono::ManagedObject* owner, mono::ManagedField* field)
		: CCProperty::Property<ReflectedField, std::string>(this, &ReflectedField::SetField, &ReflectedField::GetField),
		m_csOwner(owner), m_reflectedField(field)
	{ }
};
