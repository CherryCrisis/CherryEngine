#include "pch.hpp"

/* Mono includes */
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/profiler.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/threads.h>

#include "monowrapper.hpp"

#include <string>

using namespace mono;

namespace mono
{
	UniqueRef<ManagedType> ManagedType::int16Type = nullptr;
	UniqueRef<ManagedType> ManagedType::int32Type = nullptr;
	UniqueRef<ManagedType> ManagedType::int64Type = nullptr;
	UniqueRef<ManagedType> ManagedType::uint16Type = nullptr;
	UniqueRef<ManagedType> ManagedType::uint32Type = nullptr;
	UniqueRef<ManagedType> ManagedType::uint64Type = nullptr;
	UniqueRef<ManagedType> ManagedType::stringType = nullptr;
	UniqueRef<ManagedType> ManagedType::booleanType = nullptr;
	UniqueRef<ManagedType> ManagedType::threadType = nullptr;
	UniqueRef<ManagedType> ManagedType::singleType = nullptr;
	UniqueRef<ManagedType> ManagedType::byteType = nullptr;
	UniqueRef<ManagedType> ManagedType::arrayType = nullptr;
	UniqueRef<ManagedType> ManagedType::charType = nullptr;
	UniqueRef<ManagedType> ManagedType::doubleType = nullptr;
	UniqueRef<ManagedType> ManagedType::enumType = nullptr;
	UniqueRef<ManagedType> ManagedType::exceptionType = nullptr;
	UniqueRef<ManagedType> ManagedType::intptrType = nullptr;
	UniqueRef<ManagedType> ManagedType::uintptrType = nullptr;
	UniqueRef<ManagedType> ManagedType::sbyteType = nullptr;
	UniqueRef<ManagedType> ManagedType::voidType = nullptr;

	UniqueRef<ManagedClass> ManagedClass::int16Class = nullptr;
	UniqueRef<ManagedClass> ManagedClass::int32Class = nullptr;
	UniqueRef<ManagedClass> ManagedClass::int64Class = nullptr;
	UniqueRef<ManagedClass> ManagedClass::uint16Class = nullptr;
	UniqueRef<ManagedClass> ManagedClass::uint32Class = nullptr;
	UniqueRef<ManagedClass> ManagedClass::uint64Class = nullptr;
	UniqueRef<ManagedClass> ManagedClass::stringClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::booleanClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::threadClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::singleClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::byteClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::arrayClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::charClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::doubleClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::enumClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::exceptionClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::intptrClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::uintptrClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::sbyteClass = nullptr;
	UniqueRef<ManagedClass> ManagedClass::voidClass = nullptr;

	/* Profiler methods */
	static void Profiler_RuntimeInit(MonoProfiler* prof);
	static void Profiler_RuntimeShutdownStart(MonoProfiler* prof);
	static void Profiler_RuntimeShutdownEnd(MonoProfiler* prof);
	static void Profiler_ContextLoaded(MonoProfiler* prof, MonoAppContext* ctx);
	static void Profiler_ContextUnloaded(MonoProfiler* prof, MonoAppContext* ctx);
	static void Profiler_GCEvent(MonoProfiler* prof, MonoProfilerGCEvent ev, uint32_t gen, mono_bool isSerial);
	static void Profiler_GCAlloc(MonoProfiler* prof, MonoObject* obj);
	static void Profiler_GCResize(MonoProfiler* prof, uintptr_t size);

	//================================================================//
	//
	// Managed Assembly
	//
	//================================================================//
	ManagedAssembly::ManagedAssembly(ManagedScriptContext* ctx, const char* name, MonoImage* img, MonoAssembly* ass)
		: m_ctx(ctx), m_path(name), m_image(img), m_assembly(ass) { }

	void ManagedAssembly::PopulateReflectionInfo()
	{
		if (m_populated)
			return;

		m_populated = true;

		const MonoTableInfo* tab = mono_image_get_table_info(m_image, MONO_TABLE_TYPEDEF);

		int rows = mono_table_info_get_rows(tab);
		for (int i = 0; i < rows; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(tab, i, cols, MONO_TYPEDEF_SIZE);

			const char* ns = mono_metadata_string_heap(m_image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* c = mono_metadata_string_heap(m_image, cols[MONO_TYPEDEF_NAME]);

			m_ctx->FindClass(ns, c);

		}
	}

	/* NOTE: No info is cached here because it should be called sparingly! */
	void ManagedAssembly::GetReferencedTypes(std::vector<std::string>& refList)
	{
		const MonoTableInfo* tab = mono_image_get_table_info(m_image, MONO_TABLE_TYPEREF);

		int rows = mono_table_info_get_rows(tab);
		/* Parse all of the referenced types, and add them to the refList */
		for (int i = 0; i < rows; i++)
		{
			uint32_t cols[MONO_TYPEREF_SIZE];
			mono_metadata_decode_row(tab, i, cols, MONO_TYPEREF_SIZE);

			const char* ns = mono_metadata_string_heap(m_image, cols[MONO_TYPEREF_NAMESPACE]);
			const char* n = mono_metadata_string_heap(m_image, cols[MONO_TYPEREF_NAME]);
			char type[512];

			snprintf(type, sizeof(type), "%s.%s", ns, n);
			refList.push_back((const char*)type);
		}
	}

	bool ManagedAssembly::ValidateAgainstWhitelist(const std::vector<std::string>& whiteList)
	{
		std::vector<std::string> refTypes;
		GetReferencedTypes(refTypes);

		for (auto& type : refTypes)
		{
			bool found = false;
			for (auto& wType : whiteList)
			{
				if (type == wType)
					found = true;
			}

			if (!found)
				return false;
		}
		return true;
	}

	void ManagedAssembly::DisposeReflectionInfo()
	{
		m_classes.clear();
	}

	void ManagedAssembly::Unload()
	{
		InvalidateHandle();
		DisposeReflectionInfo();
	}

	void ManagedAssembly::InvalidateHandle()
	{
		ManagedBase::InvalidateHandle();
		for (auto& kv : m_classes)
			kv.second->InvalidateHandle();
	}

	void ManagedAssembly::Reload()
	{
		m_assembly = mono_domain_assembly_open(m_ctx->m_domain, m_path.c_str());
		m_image = mono_assembly_get_image(m_assembly);

		for (auto& [className, mClass] : m_classes)
			mClass->Reload();
	}

	void ManagedAssembly::Clear()
	{
		for (auto& [className, classRef] : m_classes)
			classRef->Clear();
	}


	void ManagedAssembly::ReportException(MonoObject* exc)
	{
		m_ctx->ReportException(exc, *this);
	}

	//================================================================//
	//
	// Managed Type
	//
	//================================================================//

	ManagedType::ManagedType(MonoType* type)
		: m_type(type)
	{
		m_isVoid = mono_type_is_void(type);
		m_isStruct = mono_type_is_struct(type);
		m_isRef = mono_type_is_reference(type);
		m_isPtr = mono_type_is_pointer(type);
		m_typeindex = mono_type_get_type(type);
	}

	void ManagedType::InitializeStatics()
	{
		int16Type		= std::make_unique<ManagedType>(ManagedClass::GetInt16()->RawType());
		int32Type		= std::make_unique<ManagedType>(ManagedClass::GetInt32()->RawType());
		int64Type		= std::make_unique<ManagedType>(ManagedClass::GetInt64()->RawType());
		uint16Type		= std::make_unique<ManagedType>(ManagedClass::GetUint16()->RawType());
		uint32Type		= std::make_unique<ManagedType>(ManagedClass::GetUint32()->RawType());
		uint64Type		= std::make_unique<ManagedType>(ManagedClass::GetUint64()->RawType());
		stringType		= std::make_unique<ManagedType>(ManagedClass::GetString()->RawType());
		booleanType		= std::make_unique<ManagedType>(ManagedClass::GetBoolean()->RawType());
		threadType		= std::make_unique<ManagedType>(ManagedClass::GetThread()->RawType());
		singleType		= std::make_unique<ManagedType>(ManagedClass::GetSingle()->RawType());
		byteType		= std::make_unique<ManagedType>(ManagedClass::GetByte()->RawType());
		arrayType		= std::make_unique<ManagedType>(ManagedClass::GetArray()->RawType());
		charType		= std::make_unique<ManagedType>(ManagedClass::GetChar()->RawType());
		doubleType		= std::make_unique<ManagedType>(ManagedClass::GetDouble()->RawType());
		enumType		= std::make_unique<ManagedType>(ManagedClass::GetEnum()->RawType());
		exceptionType	= std::make_unique<ManagedType>(ManagedClass::GetException()->RawType());
		intptrType		= std::make_unique<ManagedType>(ManagedClass::GetIntptr()->RawType());
		uintptrType		= std::make_unique<ManagedType>(ManagedClass::GetUintptr()->RawType());
		sbyteType		= std::make_unique<ManagedType>(ManagedClass::GetSbyte()->RawType());
		voidType		= std::make_unique<ManagedType>(ManagedClass::GetVoid()->RawType());
	}

	bool ManagedType::Equals(const ManagedType* other) const
	{
		return m_typeindex == other->m_typeindex;
	}

	const std::string& ManagedType::Name()
	{
		if (!m_name.empty())
			return m_name;

		char* c = mono_type_get_name(m_type);
		m_name = c;
		mono_free(c);

		return m_name;
	}

	void ManagedType::Reload()
	{
		m_isVoid = mono_type_is_void(m_type);
		m_isStruct = mono_type_is_struct(m_type);
		m_isRef = mono_type_is_reference(m_type);
		m_isPtr = mono_type_is_pointer(m_type);
		m_typeindex = mono_type_get_type(m_type);
	}

	//================================================================//
	//
	// Managed Method
	//
	//================================================================//
	ManagedMethod::ManagedMethod(MonoMethod* method, ManagedClass* cls)
	{
		if (!method)
			return;

		m_method = method;
		m_attrInfo = mono_custom_attrs_from_method(method);
		m_token = mono_method_get_token(method);
		m_class = cls;

		if (m_token)
			m_signature = mono_method_get_signature(m_method, m_class->m_assembly->m_image, m_token);

		m_name = mono_method_get_name(m_method);
		m_fullyQualifiedName = mono_method_full_name(m_method, true);
		m_paramCount = mono_signature_get_param_count(m_signature);

		m_returnType = std::make_unique<ManagedType>(mono_signature_get_return_type(m_signature));

		void* paramIter = nullptr;
		while (MonoType* paramType = mono_signature_get_params(m_signature, &paramIter))
			m_params.push_back(std::make_unique<ManagedType>(paramType));
	}

	ManagedMethod::~ManagedMethod()
	{
		if (m_attrInfo)
			mono_custom_attrs_free(m_attrInfo);

		m_returnType = nullptr;
		m_params.clear();
	}

	ManagedAssembly& ManagedMethod::Assembly() const
	{
		return *m_class->m_assembly;
	}

	void ManagedMethod::InvalidateHandle()
	{
		ManagedBase::InvalidateHandle();
		m_returnType->InvalidateHandle();
		for (auto& parm : m_params)
			parm->InvalidateHandle();

		for (auto& a : m_attributes)
			a->InvalidateHandle();
	}

	bool ManagedMethod::MatchSignature(MonoMethod* method)
	{
		return mono_method_full_name(method, true) == m_fullyQualifiedName;
	}

	bool ManagedMethod::MatchSignature(MonoType* returnval, std::vector<MonoType*> params)
	{
		/* Pre-verification that the params are likely to be equal */
		if (m_paramCount != params.size())
			return false;

		MonoType* type = mono_signature_get_return_type(m_signature);
		if (!mono_metadata_type_equal(type, returnval))
			return false;

		void* iter = nullptr;
		type = nullptr;
		int i = 0;
		while (type = mono_signature_get_params(m_signature, &iter))
		{
			if (!mono_metadata_type_equal(type, params[i]))
				return false;
			i++;
		}

		return true;
	}

	bool ManagedMethod::MatchSignature(std::vector<MonoType*> params)
	{
		if (m_paramCount != params.size())
			return false;

		// Verify that return type is VOID
		// quite messy indeed
		MonoType* type = mono_signature_get_return_type(m_signature);
		if (!mono_metadata_type_equal(type, mono_class_get_type(mono_get_void_class())))
			return false;

		void* iter = nullptr;
		type = nullptr;
		auto paramIt = params.begin();
		while ((type = mono_signature_get_params(m_signature, &iter)) && paramIt != params.end())
		{
			if (!mono_metadata_type_equal(type, *paramIt))
				return false;

			paramIt++;
		}
		return true;
	}

	bool ManagedMethod::MatchSignature()
	{
		MonoType* type = mono_signature_get_return_type(m_signature);
		if (!mono_metadata_type_equal(type, mono_class_get_type(mono_get_void_class())))
			return false;

		// Check param count is 0
		return mono_signature_get_param_count(m_signature) == 0;
	}

	MonoObject* ManagedMethod::Invoke(ManagedObject* obj, void** params, MonoObject** _exc)
	{
		MonoObject* exception = nullptr;
		MonoObject* o = mono_runtime_invoke(m_method, obj->RawObject(), params, _exc ? _exc : &exception);

		if (exception)
		{
			m_class->m_assembly->ReportException(exception);
			return nullptr;
		}

		return o;
	}

	MonoObject* ManagedMethod::InvokeStatic(void** params, MonoObject** _exc)
	{
		MonoObject* exception = nullptr;
		MonoObject* o = mono_runtime_invoke(m_method, nullptr, params, _exc ? _exc : &exception);

		if (exception)
		{
			m_class->m_assembly->ReportException(exception);
			return nullptr;
		}

		return o;
	}

	bool ManagedMethod::Reload()
	{
		m_method = nullptr;

		void* iter = nullptr;
		MonoMethod* method;
		while ((method = mono_class_get_methods(m_class->m_class, &iter)) && !m_method)
		{
			if (mono_method_full_name(method, true) != m_fullyQualifiedName)
				continue;

			m_method = method;
		}

		if (!m_method)
		{
			void* parentIter = nullptr;

			MonoClass* currentClass = m_class->m_class;
			MonoClass* parent = nullptr;
			while ((parent = mono_class_get_parent(currentClass)) && !m_method)
			{
				while ((method = mono_class_get_methods(parent, &parentIter)) && !m_method)
				{
					const char* methodName = mono_method_get_name(method);
					if (strcmp(methodName, ".ctor") == 0)
						continue;

					if (!MatchSignature(method))
						continue;

					m_method = method;
				}

				parentIter = nullptr;
				currentClass = parent;
			}

			if (!m_method)
				return false;
		}

		m_signature = mono_method_signature(m_method);
		m_attrInfo = mono_custom_attrs_from_method(m_method);
		m_token = mono_method_get_token(m_method);

		for (auto& thunk : m_thunks)
		{
			thunk->m_method = RawMethod();
			thunk->Reload();
		}

		for (auto& parm : m_params)
			parm->Reload();

		for (auto& a : m_attributes)
			a->Reload();

		return true;
	}

	//================================================================//
	//
	// Managed Field
	//
	//================================================================//

	ManagedField::ManagedField(MonoClassField* fld, ManagedClass* cls, const char* name)
		: m_class(cls), m_field(fld), m_name(name), m_type(mono_field_get_type(fld))
	{
	}

	//================================================================//
	//
	// Managed Property
	//
	//================================================================//

	ManagedProperty::ManagedProperty(MonoProperty* prop, ManagedClass* cls, const char* name)
		: m_class(cls), m_property(prop), m_name(name)
	{
		m_getMethod = mono_property_get_get_method(m_property);
		m_setMethod = mono_property_get_set_method(m_property);
	}

	//================================================================//
	//
	// Managed Class
	//
	//================================================================//
	ManagedClass::ManagedClass(MonoClass* cls)
		: m_class(cls)
	{
		m_className = mono_class_get_name(cls);
		m_namespaceName = mono_class_get_namespace(cls);
		m_attrInfo = mono_custom_attrs_from_class(cls);

		if (!m_attrInfo || !mono_custom_attrs_has_attr(m_attrInfo, cls))
			return;

		if (MonoObject* obj = mono_custom_attrs_get_attr(m_attrInfo, cls))
			m_attributes.push_back(std::make_unique<ManagedObject>(obj, *this));
	}

	ManagedClass::ManagedClass(ManagedAssembly* assembly, MonoClass* cls)
		: ManagedClass(cls)
	{
		m_assembly = assembly;

		if (m_assembly)
			PopulateReflectionInfo();
	}

	ManagedClass::~ManagedClass()
	{
		if (m_attrInfo)
			mono_custom_attrs_free(m_attrInfo);
	}

	ManagedMethod* ManagedClass::GetCtor(std::vector<MonoType*> signature)
	{
		auto itPair = m_methods.equal_range(".ctor");

		for (auto& it = itPair.first; it != itPair.second; it++)
		{
			ManagedMethod* method = it->second.get();
			if (method->MatchSignature(signature))
				return method;
		}

		return nullptr;
	}

	void ManagedClass::InitializeStatics()
	{
		int64Class		= std::make_unique<ManagedClass>(mono_get_int64_class());
		int16Class		= std::make_unique<ManagedClass>(mono_get_int16_class());
		int32Class		= std::make_unique<ManagedClass>(mono_get_int32_class());
		uint16Class		= std::make_unique<ManagedClass>(mono_get_uint16_class());
		uint32Class		= std::make_unique<ManagedClass>(mono_get_uint32_class());
		uint64Class		= std::make_unique<ManagedClass>(mono_get_uint64_class());
		stringClass		= std::make_unique<ManagedClass>(mono_get_string_class());
		booleanClass	= std::make_unique<ManagedClass>(mono_get_boolean_class());
		threadClass		= std::make_unique<ManagedClass>(mono_get_thread_class());
		singleClass		= std::make_unique<ManagedClass>(mono_get_single_class());
		byteClass		= std::make_unique<ManagedClass>(mono_get_byte_class());
		arrayClass		= std::make_unique<ManagedClass>(mono_get_array_class());
		charClass		= std::make_unique<ManagedClass>(mono_get_char_class());
		doubleClass		= std::make_unique<ManagedClass>(mono_get_double_class());
		enumClass		= std::make_unique<ManagedClass>(mono_get_enum_class());
		exceptionClass	= std::make_unique<ManagedClass>(mono_get_exception_class());
		intptrClass		= std::make_unique<ManagedClass>(mono_get_intptr_class());
		uintptrClass	= std::make_unique<ManagedClass>(mono_get_uintptr_class());
		sbyteClass		= std::make_unique<ManagedClass>(mono_get_sbyte_class());
		voidClass		= std::make_unique<ManagedClass>(mono_get_void_class());
	}

	void ManagedClass::PopulateReflectionInfo()
	{
		if (m_valid)
			return;

		void* iter = nullptr;

		m_valueClass = mono_class_is_valuetype(m_class);
		m_enumClass = mono_class_is_enum(m_class);
		m_delegateClass = mono_class_is_delegate(m_class);
		m_nullableClass = mono_class_is_nullable(m_class);
		m_size = mono_class_instance_size(m_class);
		m_alignment = mono_class_min_align(m_class);

		MonoMethod* method;
		while ((method = mono_class_get_methods(m_class, &iter)))
		{
			const char* methodName = mono_method_get_name(method);
			if (strcmp(methodName, ".ctor") == 0)
				m_numConstructors++;

			m_methods.insert({ methodName, std::make_unique<ManagedMethod>(method, this) });
		}

		void* parentIter = nullptr;

		MonoClass* currentClass = m_class;
		while (MonoClass* parent = mono_class_get_parent(currentClass))
		{
			while ((method = mono_class_get_methods(parent, &parentIter)))
			{
				const char* methodName = mono_method_get_name(method);
				if (strcmp(methodName, ".ctor") == 0)
					continue;

				m_methods.insert({ methodName, std::make_unique<ManagedMethod>(method, this) });
			}

			parentIter = nullptr;
			currentClass = parent;
		}

		MonoClassField* field;
		iter = nullptr;
		while ((field = mono_class_get_fields(m_class, &iter)))
		{
			const char* fieldName = mono_field_get_name(field);
			m_fields[fieldName] = std::make_unique<ManagedField>(field, this, fieldName);
		}

		MonoProperty* props;
		iter = nullptr;
		while (props = mono_class_get_properties(m_class, &iter))
		{
			const char* propName = mono_property_get_name(props);

			m_properties[propName] = std::make_unique<ManagedProperty>(props, this, propName);

		}

		m_populated = true;
	}
	void ManagedClass::InvalidateHandle()
	{
		ManagedBase<ManagedClass>::InvalidateHandle();
		for (auto& attr : m_attributes)
			attr->InvalidateHandle();

		for (auto& [methodName, methodRef] : m_methods)
			methodRef->InvalidateHandle();
	}

	void ManagedClass::Reload()
	{
		m_class = mono_class_from_name(m_assembly->m_image, m_namespaceName.c_str(), m_className.c_str());

		if (!m_class)
		{
			m_methods.clear();
			m_attributes.clear();
			m_handledInstances.clear();
			return;
		}

		for (auto methodIt = m_methods.begin(); methodIt != m_methods.end();)
		{
			ManagedMethod* method = methodIt->second.get();
			method->m_class = this;

			if (!method->Reload())
				methodIt = m_methods.erase(methodIt);
			else
				methodIt++;
		}

		for (auto& attr : m_attributes)
			attr->Reload();

		for (auto& instance : m_handledInstances)
			instance->Reload();
	}

	void ManagedClass::Clear()
	{
		for (auto& instance : m_handledInstances)
			instance->Dispose();
	}

	ManagedMethod* ManagedClass::FindMethod(const char* name)
	{
		auto methodIt = m_methods.find(name);
		return methodIt == m_methods.end() ? nullptr : methodIt->second.get();
	}

	ManagedField* ManagedClass::FindField(const char* name)
	{
		auto fieldIt = m_fields.find(name);
		return fieldIt == m_fields.end() ? nullptr : fieldIt->second.get();
	}

	ManagedProperty* ManagedClass::FindProperty(const char* prop)
	{
		auto propIt = m_properties.find(prop);
		return propIt == m_properties.end() ? nullptr : propIt->second.get();
	}

	/* Creates a raw instance of a this class */
	MonoObject* ManagedClass::CreateRawInstance(std::vector<MonoType*> signature, void** params)
	{
		if (!m_assembly)
			return nullptr;

		ManagedMethod* ctor = GetCtor(signature);

		if (!ctor)
			return nullptr;

		MonoObject* exception = nullptr;
		MonoObject* obj = mono_object_new(m_assembly->m_ctx->m_domain, m_class); // Allocate storage
		if (signature.size() > 0) {
			mono_runtime_invoke(ctor->m_method, obj, params, &exception);
		}
		else
		{
			mono_runtime_object_init(obj);				// Invoke default constructor
		}

		if (exception || !obj)
		{
			m_assembly->m_ctx->ReportException(obj, *m_assembly);
			return nullptr;
		}

		return obj;
	}

	/* Creates an instance of a this class */
	ManagedObject* ManagedClass::CreateInstance(std::vector<MonoType*> signature, void** params)
	{
		MonoObject* obj = CreateRawInstance(signature, params);
		if (!obj)
			return nullptr;

		UniqueRef<ManagedObject> instance = std::make_unique<ManagedObject>(obj, *this);
		ManagedObject* rawRef = instance.get();

		m_managedInstances.push_back(std::move(instance));

		return rawRef;
	}

	MonoObject* ManagedClass::CreateUnmanagedRawInstance(void* cPtr, bool ownMemory)
	{
		void* args[] = { &cPtr, &ownMemory };
		return CreateRawInstance({ ManagedType::GetIntptr()->RawType(), ManagedType::GetBoolean()->RawType() }, args);
	}

	ManagedObject* ManagedClass::CreateUnmanagedInstance(void* cPtr, bool ownMemory)
	{
		void* args[] = { &cPtr, &ownMemory };
		MonoObject* obj = CreateRawInstance({ ManagedType::GetIntptr()->RawType(), ManagedType::GetBoolean()->RawType() }, args);

		if (!obj)
			return nullptr;

		UniqueRef<ManagedObject> instance = std::make_unique<ManagedObject>(obj, *this);

		if (!instance)
			return nullptr;

		instance->m_handledPtr = cPtr;

		ManagedObject* rawRef = instance.get();

		m_handledInstances.push_back(std::move(instance));

		return rawRef;
	}

	mono_byte ManagedClass::NumConstructors() const {
		return m_numConstructors;
	}

	bool ManagedClass::ImplementsInterface(ManagedClass& interface) {
		return mono_class_implements_interface(m_class, interface.m_class);
	}

	bool ManagedClass::DerivedFromClass(ManagedClass& cls) {
		return mono_class_is_subclass_of(m_class, cls.m_class, true);
	}

	bool ManagedClass::DerivedFromClass(MonoClass& cls) {
		return mono_class_is_subclass_of(m_class, &cls, true);
	}

	bool ManagedClass::IsVoid() {
		return m_class == mono_get_void_class();
	}

	bool ManagedClass::IsInt16() {
		return m_class == mono_get_int16_class();
	}

	bool ManagedClass::IsInt32() {
		return m_class == mono_get_int32_class();
	}

	bool ManagedClass::IsInt64() {
		return m_class == mono_get_int64_class();
	}

	bool ManagedClass::IsDouble() {
		return m_class == mono_get_double_class();
	}

	bool ManagedClass::IsIntptr() {
		return m_class == mono_get_intptr_class();
	}

	bool ManagedClass::IsThread() {
		return m_class == mono_get_thread_class();
	}

	bool ManagedClass::IsArray() {
		return m_class == mono_get_array_class();
	}

	bool ManagedClass::IsByte() {
		return m_class == mono_get_byte_class();
	}

	bool ManagedClass::IsChar() {
		return m_class == mono_get_char_class();
	}

	bool ManagedClass::IsUInt32() {
		return m_class == mono_get_uint32_class();
	}

	bool ManagedClass::IsUInt16() {
		return m_class == mono_get_uint32_class();
	}

	bool ManagedClass::IsUInt64() {
		return m_class == mono_get_uint64_class();
	}

	bool ManagedClass::IsUIntptr() {
		return m_class == mono_get_uintptr_class();
	}

	bool ManagedClass::IsBool() {
		return m_class == mono_get_boolean_class();
	}

	//================================================================//
	//
	// Managed Object
	//
	//================================================================//

	ManagedObject::ManagedObject(MonoObject* obj, ManagedClass& cls, EManagedObjectHandleType type) {
		m_obj = obj;
		m_class = &cls;
		switch (type) {
		case EManagedObjectHandleType::HANDLE:
			m_gcHandle = mono_gchandle_new(obj, false);
			m_getObject = [this]() -> MonoObject* { return mono_gchandle_get_target(m_gcHandle); };
			break;
		case EManagedObjectHandleType::HANDLE_PINNED:
			m_gcHandle = mono_gchandle_new(obj, true);
			m_getObject = [this]() -> MonoObject* { return m_obj; };
			break;
		case EManagedObjectHandleType::WEAKREF:
			m_gcHandle = mono_gchandle_new_weakref(obj, false);
			m_getObject = [this]() -> MonoObject* { return mono_gchandle_get_target(m_gcHandle); };
			break;
		default:
			break;
		}
	}

	ManagedObject::~ManagedObject() {
		mono_gchandle_free(m_gcHandle);
	}

	bool ManagedObject::SetProperty(ManagedProperty* prop, const void* value) {
		MonoObject* exception = nullptr;
		const void* params[] = {value};

		if (!prop->m_setMethod)
			return false;

		MonoObject* res = mono_runtime_invoke(prop->m_setMethod, RawObject(), (void**)params, &exception);

		if (exception)
			return false;
		return true;
	}

	bool ManagedObject::SetField(ManagedField* prop, const void* value)
	{
		mono_field_set_value(RawObject(), prop->RawField(), (void*)value);
		return true;
	}

	bool ManagedObject::GetProperty(ManagedProperty* prop, void** outValue)
	{
		if (!prop->m_getMethod)
			return false;

		MonoObject* exception = nullptr;
		MonoObject* res = mono_runtime_invoke(prop->m_getMethod, RawObject(), NULL, &exception);

		if (!res || exception)
			return false;

		*outValue = mono_object_unbox(res);
		return true;
	}

	bool ManagedObject::GetField(ManagedField* field, void* outValue)
	{
		mono_field_get_value(RawObject(), field->RawField(), outValue);
		return true;
	}

	bool ManagedObject::SetProperty(const char* p, const void* value)
	{
		auto propIt = m_class->m_properties.find(p);

		if (propIt == m_class->m_properties.end())
			return false;

		return SetProperty(propIt->second.get(), value);
	}

	bool ManagedObject::SetField(const char* p, const void* value)
	{
		auto fieldIt = m_class->m_fields.find(p);

		if (fieldIt == m_class->m_fields.end())
			return false;

		return SetField(fieldIt->second.get(), value);
	}

	bool ManagedObject::GetProperty(const char* p, void** outValue)
	{
		auto propIt = m_class->m_properties.find(p);

		if (propIt == m_class->m_properties.end())
			return false;

		return GetProperty(propIt->second.get(), outValue);
	}

	bool ManagedObject::GetField(const char* p, void* outValue)
	{
		if (ManagedField* field = m_class->FindField(p))
			return GetField(field, outValue);

		return false;
	}

	void ManagedObject::Dispose()
	{
		auto csDispose = (void(*)(MonoObject*, bool, MonoException**))mono_method_get_unmanaged_thunk(m_class->FindMethod("Dispose")->RawMethod());

		// TODO: Add real excep
		MonoException* excep = nullptr;
		csDispose(RawObject(), true, &excep);
	}

	void ManagedObject::Reload()
	{
		m_obj = m_class->CreateUnmanagedRawInstance(m_handledPtr, false);
	}

	MonoObject* ManagedObject::Invoke(ManagedMethod* method, void** params) {
		return method->Invoke(this, params);
	}

	//================================================================//
	//
	// Managed Script Context
	//
	//================================================================//

	ManagedScriptContext::ManagedScriptContext(char* domainName, const char* path, const char* baseImage)
		: m_domainName(domainName), m_path(path), m_baseImage(baseImage) { }

	ManagedScriptContext::~ManagedScriptContext()
	{
		// TODO: ca explose mais je sais pas pourquoi

		/*for (auto& a : m_loadedAssemblies)
		{
			if (a->m_assembly)
				mono_assembly_close(a->m_assembly);

			//if (a->m_image)
			//	mono_image_close(a->m_image);

		}*/
	}

	bool ManagedScriptContext::Init()
	{
		m_domain = mono_domain_create_appdomain(&m_domainName[0], nullptr);

		if (!m_domain)
			return false;

		mono_domain_set(m_domain, 0);

		for (auto& assembly : m_loadedAssemblies)
			assembly->Reload();

		LoadAssembly(m_baseImage.c_str());

		return true;
	}

	bool ManagedScriptContext::Unload()
	{
		MonoDomain* rootDomain = mono_get_root_domain();
		if (m_domain == rootDomain)
			return false;

		for (auto& assembly : m_loadedAssemblies)
			assembly->Clear();

		mono_domain_set(rootDomain, 0);
		mono_domain_unload(m_domain);

		return true;
	}

	bool ManagedScriptContext::Reload()
	{
		if (!Unload())
			return false;

		return Init();
	}

	bool ManagedScriptContext::LoadAssembly(const char* path)
	{
		if (!m_domain)
			return false;

		MonoAssembly* ass = mono_domain_assembly_open(m_domain, path);

		if (!ass)
			return false;

		MonoImage* img = mono_assembly_get_image(ass);

		if (!img)
			return false;

		UniqueRef<ManagedAssembly> newAss = std::make_unique<ManagedAssembly>(this, path, img, ass);
		newAss->PopulateReflectionInfo();

		m_loadedAssemblies.push_back(std::move(newAss));
		return true;
	}

	bool ManagedScriptContext::UnloadAssembly(const char* name)
	{
		for (auto it = m_loadedAssemblies.begin(); it != m_loadedAssemblies.end(); ++it)
		{
			if ((*it)->m_path != name)
				continue;

			if ((*it)->m_image)
				mono_image_close((*it)->m_image);

			if ((*it)->m_assembly)
				mono_assembly_close((*it)->m_assembly);

			m_loadedAssemblies.erase(it);
			return true;
		}
		return false;
	}

	/* Performs a class search in all loaded assemblies */
	/* If you have the assembly name, please use the alternative version of this
	 * function */
	ManagedClass* ManagedScriptContext::FindClass(const char* ns, const char* cls)
	{
		/* Try to find the managed class in each of the assemblies. if found, create
		 * the managed class and return */
		/* Also check the hashmap we have setup */
		for (auto& a : m_loadedAssemblies)
		{
			if (!a)
				continue;

			if (ManagedClass* _cls = FindClass(a.get(), ns, cls))
				return _cls;
		}

		return nullptr;
	}

	ManagedClass* ManagedScriptContext::FindClass(ManagedAssembly* assembly, const char* ns, const char* cls)
	{
		auto itpair = assembly->m_classes.equal_range(ns);

		for (auto it = itpair.first; it != itpair.second; ++it)
		{
			if (it->second->m_className == cls)
				return it->second.get();
		}

		/* Have mono perform the class lookup. If it's there, create and add a new
		 * managed class */
		MonoClass* monoClass = mono_class_from_name(assembly->m_image, ns, cls);

		if (!monoClass)
			return nullptr;

		UniqueRef<ManagedClass> classRef = std::make_unique<ManagedClass>(assembly, monoClass);
		ManagedClass* rawRef = classRef.get();
		assembly->m_classes.insert({ classRef->ClassName(), std::move(classRef)});

		return rawRef;
	}

	/* Used to locate a class not added by any assemblies explicitly loaded by the
	 * user */
	/* These assemblies are usually going to be system assemblies or members of the
	 * C# standard library */
	MonoClass* ManagedScriptContext::FindSystemClass(const char* ns, const char* cls)
	{
		struct pvt_t
		{
			ManagedScriptContext* _this;
			const char* ns;
			const char* cls;
			bool isdone;
			MonoClass* result;
		} pvt;

		pvt.cls = cls;
		pvt.ns = ns;
		pvt._this = this;
		pvt.result = nullptr;
		pvt.isdone = false;

		mono_assembly_foreach(
			[](void* ass, void* pvt)
			{
				MonoAssembly* _ass = static_cast<MonoAssembly*>(ass);
				pvt_t* _pvt = static_cast<pvt_t*>(pvt);

				if (_pvt->isdone)
					return;

				MonoImage* img = mono_assembly_get_image(_ass);
				if (!img)
					return;

				MonoClass* res = mono_class_from_name(img, _pvt->ns, _pvt->cls);

				if (!res)
					return;

				_pvt->isdone = true;
				_pvt->result = res;
			}, &pvt);

		return pvt.result;
	}

	ManagedAssembly* ManagedScriptContext::FindAssembly(const char* path)
	{
		for (auto& a : m_loadedAssemblies)
		{
			if (a->m_path == path)
				return a.get();
		}
		return nullptr;
	}

	/* Clears all reflection info stored in each assembly description */
	/* WARNING: this will invalidate your handles! */
	void ManagedScriptContext::ClearReflectionInfo()
	{
		for (auto& a : m_loadedAssemblies)
			a->m_classes.clear();
	}

	void ManagedScriptContext::PopulateReflectionInfo()
	{
		for (auto& a : m_loadedAssemblies)
			a->PopulateReflectionInfo();
	}

	bool ManagedScriptContext::ValidateAgainstWhitelist(const std::vector<std::string>& whitelist)
	{
		for (auto& a : m_loadedAssemblies)
		{
			if (!a->ValidateAgainstWhitelist(whitelist))
				return false;
		}

		return true;
	}

	void ManagedScriptContext::ReportException(MonoObject* obj, ManagedAssembly& ass)
	{
		auto exc = GetExceptionDescriptor(obj);

		for (auto& c : m_callbacks)
			c(this, &ass, obj, exc);
	}

	ManagedException_t ManagedScriptContext::GetExceptionDescriptor(MonoObject* exception)
	{
		ManagedException_t exc;

		/* Make sure that the baseclass of the exception is System.Exception. If
		 * not, we've got some weird object that we shouldn't have */
		MonoClass* cls = mono_object_get_class(exception);
		MonoClass* excClass = FindSystemClass("System", "Exception");
		if (!mono_object_isinst(exception, excClass))
			return exc;

		/* Obtain the properties by iterating through the class's props */
		MonoProperty* propMsg = mono_class_get_property_from_name(cls, "Message");
		MonoProperty* propSrc = mono_class_get_property_from_name(cls, "Source");
		MonoProperty* propST = mono_class_get_property_from_name(cls, "StackTrace");

		/* Obtain the properties */
		/* This is a big weird and unoptimized because mono I guess */
		MonoObject* msg = nullptr;
		MonoObject* src = nullptr;
		MonoObject* stack = nullptr;
		MonoObject* propexcept = nullptr;
		msg = mono_property_get_value(propMsg, exception, nullptr, &propexcept);
		src = mono_property_get_value(propSrc, exception, nullptr, &propexcept);
		stack = mono_property_get_value(propST, exception, nullptr, &propexcept);

		if (msg)
		{
			char* buf = mono_string_to_utf8(mono_object_to_string(msg, nullptr));
			exc.message = (const char*)buf;
			mono_free(buf);
		}

		if (src)
		{
			char* buf = mono_string_to_utf8(mono_object_to_string(src, nullptr));
			exc.source = (const char*)buf;
			mono_free(buf);
		}

		if (stack)
		{
			char* buf = mono_string_to_utf8(mono_object_to_string(stack, nullptr));
			exc.stackTrace = (const char*)buf;
			mono_free(buf);
		}

		exc.klass = mono_class_get_name(cls);
		exc.ns = mono_class_get_namespace(cls);

		MonoObject* pexc = nullptr;
		char* str = mono_string_to_utf8(mono_object_to_string(exception, &pexc));

		if (!pexc && str)
			exc.string_rep = (const char*)str;

		if (str)
			mono_free(str);

		return exc;
	}

	//================================================================//
	//
	// Managed Script System
	//
	//================================================================//
	ManagedScriptSystem::ManagedScriptSystem(ManagedScriptSystemSettings_t settings)
		: m_settings(settings)
	{
		/* Basically just a guard to ensure we dont have multiple per process */
		static bool g_managedScriptSystemExists = false;
		if (g_managedScriptSystemExists)
		{
			printf("A managed script system already exists!\n\tThere can only be "
				   "one per process because of how mono works.\n\tPlease fix your "
				   "program so only one script system is made\n\n");
		}

		g_managedScriptSystemExists = true;

		/*if (settings.configIsFile)
			mono_config_parse(settings.configData);
		else
			mono_config_parse_memory(settings.configData);*/

		/* Push a root profiling context */
		PushProfilingContext();

		/* Create and register the new profiler */
		m_monoProfiler.handle = mono_profiler_create(&m_monoProfiler);
		m_monoProfiler.scriptsys = this;
		mono_profiler_set_runtime_initialized_callback(m_monoProfiler.handle, Profiler_RuntimeInit);
		mono_profiler_set_runtime_shutdown_begin_callback(m_monoProfiler.handle, Profiler_RuntimeShutdownStart);
		mono_profiler_set_runtime_shutdown_end_callback(m_monoProfiler.handle, Profiler_RuntimeShutdownEnd);
		mono_profiler_set_gc_allocation_callback(m_monoProfiler.handle, Profiler_GCAlloc);
		mono_profiler_set_gc_event_callback(m_monoProfiler.handle, Profiler_GCEvent);
		mono_profiler_set_gc_resize_callback(m_monoProfiler.handle, Profiler_GCResize);
		mono_profiler_set_context_loaded_callback(m_monoProfiler.handle, Profiler_ContextLoaded);
		mono_profiler_set_context_unloaded_callback(m_monoProfiler.handle, Profiler_ContextUnloaded);

		/* Register our memory allocator for mono */
		m_allocator = {MONO_ALLOCATOR_VTABLE_VERSION, settings._malloc, settings._realloc, settings._free, settings._calloc};
		mono_set_allocator_vtable(&m_allocator);

		// Create a SINGLE jit environment!
		m_rootDomain = mono_jit_init(settings.scriptSystemDomainName);

		ManagedClass::InitializeStatics();
		ManagedType::InitializeStatics();
	}

	ManagedScriptSystem::~ManagedScriptSystem()
	{
		m_contexts.clear();
		mono_jit_cleanup(m_rootDomain);
	}

	ManagedScriptContext* ManagedScriptSystem::CreateContext(char* domainName, const char* path, const char* image)
	{
		UniqueRef<ManagedScriptContext> ctx = std::make_unique<ManagedScriptContext>(domainName, path, image);

		if (!ctx->Init())
			return nullptr;

		ManagedScriptContext* rawRef = ctx.get();
		m_contexts.insert({ domainName, std::move(ctx) });

		return rawRef;
	}

	void ManagedScriptSystem::DestroyContext(ManagedScriptContext* ctx)
	{
		m_contexts.erase(ctx->m_domainName);
	}

	uint64_t ManagedScriptSystem::HeapSize() const
	{
		return mono_gc_get_heap_size();
	}

	uint64_t ManagedScriptSystem::UsedHeapSize() const
	{
		return mono_gc_get_used_size();
	}

	void ManagedScriptSystem::RegisterNativeFunction(const char* name, void* func)
	{
		mono_add_internal_call(name, func);
	}

	void ManagedScriptSystem::ReportProfileStats()
	{
		MonoProfiler* prof = &m_monoProfiler;
	}

	uint32_t ManagedScriptSystem::MaxGCGeneration()
	{
		return mono_gc_max_generation();
	}

	void ManagedScriptSystem::RunGCCollect(uint32_t gen)
	{
		mono_gc_collect(gen);
	}

	void ManagedScriptSystem::RunGCCollectAll()
	{
		for (int i = 0; i < mono_gc_max_generation(); i++)
			mono_gc_collect(i);
	}

	void ManagedScriptSystem::PushProfilingContext()
	{
		m_profilingData.push(ManagedProfilingData_t());
		m_curFrame = &m_profilingData.top();
	}

	void ManagedScriptSystem::PopProfilingContext()
	{
		/* There should always be at least one frame in the stack */
		if (m_profilingData.size() > 1)
		{
			m_profilingData.pop();
			m_curFrame = &m_profilingData.top();
		}
	}

	void ManagedScriptSystem::SetProfilingSettings(ManagedProfilingSettings_t settings) {
		m_profilingSettings = settings;
		if (m_profilingSettings.profileAllocations)
			mono_profiler_enable_allocations();
	}

	static void Profiler_RuntimeInit(MonoProfiler* prof) { }

	static void Profiler_RuntimeShutdownStart(MonoProfiler* prof)
	{
		auto& ctx = prof->scriptsys->CurrentProfilingData();
	}

	static void Profiler_RuntimeShutdownEnd(MonoProfiler* prof)
	{
		auto& ctx = prof->scriptsys->CurrentProfilingData();
	}

	static void Profiler_ContextLoaded(MonoProfiler* prof, MonoAppContext* _ctx)
	{
		auto& ctx = prof->scriptsys->CurrentProfilingData();
		ctx.totalContextLoads++;
	}

	static void Profiler_ContextUnloaded(MonoProfiler* prof, MonoAppContext* _ctx)
	{
		auto& ctx = prof->scriptsys->CurrentProfilingData();
		ctx.totalContextUnloads++;
	}

	static void Profiler_GCEvent(MonoProfiler* prof, MonoProfilerGCEvent ev, uint32_t gen, mono_bool isSerial)
	{
		auto& ctx = prof->scriptsys->CurrentProfilingData();
	}

	static void Profiler_GCAlloc(MonoProfiler* prof, MonoObject* obj)
	{
		auto& ctx = prof->scriptsys->CurrentProfilingData();
		ctx.bytesAlloc += mono_object_get_size(obj);
		ctx.totalAllocs++;
	}

	static void Profiler_GCResize(MonoProfiler* prof, uintptr_t size)
	{
		auto& ctx = prof->scriptsys->CurrentProfilingData();
		ctx.totalMoves++;
		ctx.bytesMoved += size;
	}
}