#pragma once

#include "csreflection.hpp"

#include <locale>
#include <codecvt>
#include <cstdlib>

template <>
struct ReflectedField<std::string> : public CCProperty::ConstRefProperty<ReflectedField<std::string>, std::string>
{
private:
	mono::ManagedField* m_reflectedField = nullptr;
	mono::ManagedObject* m_csOwner = nullptr;

public:
	void SetField(const std::string& value)
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
		: CCProperty::ConstRefProperty<ReflectedField, std::string>(this, &ReflectedField::SetField, &ReflectedField::GetField),
		m_csOwner(owner), m_reflectedField(field)
	{ }
};


template <>
struct ReflectedField<Object*> : public CCProperty::CopyProperty<ReflectedField<Object*>, Object*>
{
private:
	mono::ManagedField* m_reflectedField = nullptr;
	mono::ManagedObject* m_csOwner = nullptr;
	mono::ManagedClass* m_baseClass = nullptr;

	MonoMethod* m_handleMethod = nullptr;
	mono::ManagedMethod* m_managedGetCPtr = nullptr;

public:
	void SetField(Object* value)
	{
		mono::ManagedObject* managedInstance = m_baseClass->CreateUnmanagedInstance(value, false);
		m_csOwner->SetField(m_reflectedField, managedInstance->RawObject());
	}

	Object* GetField()
	{
		MonoObject* managedFieldInstance = nullptr, *excep = nullptr;
		m_csOwner->GetField(m_reflectedField, &managedFieldInstance);

		MonoObject* ptrHandle = m_managedGetCPtr->InvokeStatic((void**)&managedFieldInstance, &excep);

		void* unboxedHandle = mono_object_unbox(ptrHandle);
		MonoObject* res = mono_runtime_invoke(m_handleMethod, unboxedHandle, nullptr, &excep);

		if (!res || excep)
			return nullptr;

		return *(Object**)mono_object_unbox(res);
	}

	ReflectedField(mono::ManagedObject* owner, mono::ManagedField* field, mono::ManagedClass* baseClass, MonoMethod* handleMethod)
		: CCProperty::CopyProperty<ReflectedField, Object*>(this, &ReflectedField::SetField, &ReflectedField::GetField),
		m_csOwner(owner), m_reflectedField(field), m_baseClass(baseClass), m_handleMethod(handleMethod)
	{
		m_managedGetCPtr = m_baseClass->FindMethod("getCPtr");
	}
};
