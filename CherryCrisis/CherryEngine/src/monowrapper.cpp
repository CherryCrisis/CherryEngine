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

#include <assert.h>
#include <string.h>

#ifndef ASSERT
#define ASSERT(x) assert(x)
#endif

using namespace mono;

struct _MonoProfiler
{
	MonoProfilerHandle handle;
	uint64_t totalAllocs;
	uint64_t totalMoves;
	uint64_t bytesMoved;
	uint64_t bytesAlloc;
	mono::ManagedScriptSystem* scriptsys;
};

namespace mono {

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

	MonoDomain* g_jitDomain;

	MonoProfiler g_monoProfiler;

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
	ManagedAssembly::ManagedAssembly(ManagedScriptContext* ctx, const std::string& name, MonoImage* img, MonoAssembly* ass)
		: m_ctx(ctx), m_path(name), m_image(img), m_assembly(ass), m_populated(false) { }

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

	void ManagedAssembly::ReportException(MonoObject* exc)
	{
		m_ctx->ReportException(*exc, *this);
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
		return mono_type_get_type(m_type) == mono_type_get_type(other->m_type);
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

	//================================================================//
	//
	// Managed Method
	//
	//================================================================//

	ManagedMethod::ManagedMethod(MonoMethod* method, ManagedClass* cls)
		: m_populated(false)
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
		m_paramCount = mono_signature_get_param_count(m_signature);

		m_returnType = std::make_shared<ManagedType>(mono_signature_get_return_type(m_signature));

		void* paramIter = nullptr;
		while (MonoType* paramType = mono_signature_get_params(m_signature, &paramIter))
			m_params.push_back(std::make_shared<ManagedType>(paramType));
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

	ManagedClass& ManagedMethod::Class() const
	{
		return *m_class;
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
		while ((type = mono_signature_get_params(m_signature, &iter)))
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
		int i = 0;
		while ((type = mono_signature_get_params(m_signature, &iter)))
		{
			if (!mono_metadata_type_equal(type, params[i]))
				return false;

			i++;
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

	//================================================================//
	//
	// Managed Field
	//
	//================================================================//

	ManagedField::ManagedField(MonoClassField* fld, ManagedClass& cls, const char* name)
		: m_class(cls), m_field(fld), m_name(name), m_type(mono_field_get_type(fld))
	{
	}

	//================================================================//
	//
	// Managed Property
	//
	//================================================================//

	ManagedProperty::ManagedProperty(MonoProperty& prop, ManagedClass& cls, const char* name)
		: m_class(cls), m_property(&prop), m_name(name)
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
			m_attributes.push_back(std::make_shared<ManagedObject>(obj, *this));
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
			if (strcmp(mono_method_get_name(method), ".ctor") == 0)
				m_numConstructors++;
			m_methods.push_back(std::make_shared<ManagedMethod>(method, this));
		}

		void* parentIter = nullptr;

		MonoClass* currentClass = m_class;
		while (MonoClass* parent = mono_class_get_parent(currentClass))
		{
			while ((method = mono_class_get_methods(parent, &parentIter)))
			{
				if (strcmp(mono_method_get_name(method), ".ctor") == 0)
					continue;

				m_methods.push_back(std::make_shared<ManagedMethod>(method, this));
			}

			parentIter = nullptr;
			currentClass = parent;
		}

		MonoClassField* field;
		iter = nullptr;
		while ((field = mono_class_get_fields(m_class, &iter)))
		{
			const char* fieldName = mono_field_get_name(field);
			m_fields[fieldName] = std::make_shared<ManagedField>(field, *this, fieldName);
		}

		MonoProperty* props;
		iter = nullptr;
		while (props = mono_class_get_properties(m_class, &iter))
		{
			const char* propName = mono_property_get_name(props);

			m_properties[propName] = std::make_shared<ManagedProperty>(*props, *this, propName);

		}

		m_populated = true;
	}
	void ManagedClass::InvalidateHandle()
	{
		ManagedBase<ManagedClass>::InvalidateHandle();
		for (auto& attr : m_attributes)
			attr->InvalidateHandle();

		for (auto& meth : m_methods)
			meth->InvalidateHandle();
	}

	void ManagedClass::Reload()
	{
	
	}


	// TODO: Investigate perf of this, maybe use a hashmap? Might just be faster to
	// not though.
	Ref<ManagedMethod> ManagedClass::FindMethod(const std::string& name)
	{
		for (auto m : m_methods)
			if (m->m_name == name)
				return m;

		return nullptr;
	}

	Ref<ManagedField> ManagedClass::FindField(const std::string& name)
	{
		auto fieldIt = m_fields.find(name);

		if (fieldIt == m_fields.end())
			return nullptr;

		return fieldIt->second;
	}

	Ref<ManagedProperty> ManagedClass::FindProperty(const std::string& prop)
	{
		auto propIt = m_properties.find(prop);

		if (propIt == m_properties.end())
			return nullptr;

		return propIt->second;
	}

	/* Creates an instance of a this class */
	Ref<ManagedObject> ManagedClass::CreateInstance(std::vector<MonoType*> signature, void** params)
	{
		if (!m_assembly)
			return nullptr;

		for (auto& method : m_methods) {
			if (method->m_name == ".ctor" && method->MatchSignature(signature)) {
				MonoObject* exception = nullptr;
				MonoObject* obj = mono_object_new(m_assembly->m_ctx->m_domain,
												  m_class); // Allocate storage
				if (signature.size() > 0) {
					mono_runtime_invoke(method->m_method, obj, params, &exception);
				}
				else
				{
					mono_runtime_object_init(obj);				// Invoke default constructor
				}

				if (exception || !obj) {
					m_assembly->m_ctx->ReportException(*obj, *m_assembly);
					return nullptr;
				}
				return std::make_shared<ManagedObject>(obj, *this);
			}
		}

		return nullptr;
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

	bool ManagedObject::SetProperty(ManagedProperty* prop, void* value) {
		MonoObject* exception = nullptr;
		void* params[] = {value};

		if (!prop->m_setMethod)
			return false;

		MonoObject* res = mono_runtime_invoke(prop->m_setMethod, RawObject(), params, &exception);

		if (exception)
			return false;
		return true;
	}

	bool ManagedObject::SetField(ManagedField* prop, void* value)
	{
		mono_field_set_value(RawObject(), prop->RawField(), value);
		return true;
	}

	bool ManagedObject::GetProperty(ManagedProperty* prop, void** outValue) {
		MonoObject* exception = nullptr;
		void* params[] = {outValue};

		if (!prop->m_getMethod)
			return false;

		MonoObject* res = mono_runtime_invoke(prop->m_getMethod, RawObject(), NULL, &exception);

		if (!res || exception) {
			return false;
		}

		*outValue = mono_object_unbox(res);
		return true;
	}

	bool ManagedObject::GetField(ManagedField* prop, void* outValue)
	{
		mono_field_get_value(RawObject(), prop->RawField(), outValue);
		return true;
	}

	bool ManagedObject::SetProperty(const std::string& p, void* value)
	{
		auto propIt = m_class->m_properties.find(p);

		if (propIt == m_class->m_properties.end())
			return false;

		return SetProperty(propIt->second.get(), value);
	}

	bool ManagedObject::SetField(const std::string& p, void* value)
	{
		auto fieldIt = m_class->m_fields.find(p);

		if (fieldIt == m_class->m_fields.end())
			return false;

		return SetField(fieldIt->second.get(), value);
	}

	bool ManagedObject::GetProperty(const std::string& p, void** outValue)
	{
		auto propIt = m_class->m_properties.find(p);

		if (propIt == m_class->m_properties.end())
			return false;

		return GetProperty(propIt->second.get(), outValue);
	}

	bool ManagedObject::GetField(const std::string& p, void* outValue)
	{
		auto fieldIt = m_class->m_fields.find(p);

		if (fieldIt == m_class->m_fields.end())
			return false;

		return GetField(fieldIt->second.get(), outValue);
	}

	MonoObject* ManagedObject::Invoke(struct ManagedMethod* method, void** params) {
		return method->Invoke(this, params);
	}

	//================================================================//
	//
	// Managed Script Context
	//
	//================================================================//

	ManagedScriptContext::ManagedScriptContext(char* domainName, const std::string& baseImage)
		: m_domainName(domainName), m_baseImage(baseImage) { }

	ManagedScriptContext::~ManagedScriptContext()
	{
		for (auto& a : m_loadedAssemblies)
		{
			if (a->m_image)
				mono_image_close(a->m_image);

			if (a->m_assembly)
				mono_assembly_close(a->m_assembly);
		}
	}

	bool ManagedScriptContext::Init()
	{
		m_domain = mono_domain_create_appdomain(&m_domainName[0], nullptr);

		if (!m_domain)
			return false;

		mono_domain_set(m_domain, 0);

		for (auto& a : m_loadedAssemblies)
			a->Reload();

		LoadAssembly(m_baseImage.c_str());

		m_initialized = true;
		return true;
	}

	bool ManagedScriptContext::Unload()
	{
		if (m_domain == g_jitDomain)
			return false;

		mono_domain_set(g_jitDomain, 0);
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
		if (!img) {
			return false;
		}
		Ref<ManagedAssembly> newAss = std::make_shared<ManagedAssembly>(this, path, img, ass);
		m_loadedAssemblies.push_back(newAss);
		newAss->PopulateReflectionInfo();
		return true;
	}

	bool ManagedScriptContext::UnloadAssembly(const std::string& name)
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
	Ref<ManagedClass> ManagedScriptContext::FindClass(const std::string& ns, const std::string& cls)
	{
		/* Try to find the managed class in each of the assemblies. if found, create
		 * the managed class and return */
		/* Also check the hashmap we have setup */
		for (auto& a : m_loadedAssemblies) {
			if (!a)
				continue;

			if (Ref<ManagedClass> _cls = FindClass(*a, ns, cls))
				return _cls;
		}

		return nullptr;
	}

	Ref<ManagedClass> ManagedScriptContext::FindClass(ManagedAssembly& assembly, const std::string& ns, const std::string& cls)
	{
		auto itpair = assembly.m_classes.equal_range(ns);

		for (auto it = itpair.first; it != itpair.second; ++it)
		{
			if (it->second->m_className == cls)
				return it->second;
		}

		/* Have mono perform the class lookup. If it's there, create and add a new
		 * managed class */
		MonoClass* monoClass = mono_class_from_name(assembly.m_image, ns.c_str(), cls.c_str());

		if (!monoClass)
			return nullptr;

		Ref<ManagedClass> classRef = std::make_shared<ManagedClass>(&assembly, monoClass);
		assembly.m_classes.insert(std::make_pair(classRef->ClassName(), classRef));

		return classRef;
	}

	/* Used to locate a class not added by any assemblies explicitly loaded by the
	 * user */
	/* These assemblies are usually going to be system assemblies or members of the
	 * C# standard library */
	MonoClass* ManagedScriptContext::FindSystemClass(const std::string& ns, const std::string& cls)
	{
		struct pvt_t
		{
			ManagedScriptContext* _this;
			const char* ns;
			const char* cls;
			bool isdone;
			MonoClass* result;
		} pvt;

		pvt.cls = cls.c_str();
		pvt.ns = ns.c_str();
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

	Ref<ManagedAssembly> ManagedScriptContext::FindAssembly(const std::string& path) {
		for (auto& a : m_loadedAssemblies)
		{
			if (a->m_path == path)
				return a;
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

	void ManagedScriptContext::ReportException(MonoObject& obj, ManagedAssembly& ass) {
		auto exc = GetExceptionDescriptor(&obj);

		for (auto& c : m_callbacks)
			c(this, &ass, &obj, exc);
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
		: m_settings(settings), m_curFrame(nullptr)
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
		g_monoProfiler.handle = mono_profiler_create(&g_monoProfiler);
		g_monoProfiler.scriptsys = this;
		mono_profiler_set_runtime_initialized_callback(g_monoProfiler.handle, Profiler_RuntimeInit);
		mono_profiler_set_runtime_shutdown_begin_callback(g_monoProfiler.handle, Profiler_RuntimeShutdownStart);
		mono_profiler_set_runtime_shutdown_end_callback(g_monoProfiler.handle, Profiler_RuntimeShutdownEnd);
		mono_profiler_set_gc_allocation_callback(g_monoProfiler.handle, Profiler_GCAlloc);
		mono_profiler_set_gc_event_callback(g_monoProfiler.handle, Profiler_GCEvent);
		mono_profiler_set_gc_resize_callback(g_monoProfiler.handle, Profiler_GCResize);
		mono_profiler_set_context_loaded_callback(g_monoProfiler.handle, Profiler_ContextLoaded);
		mono_profiler_set_context_unloaded_callback(g_monoProfiler.handle, Profiler_ContextUnloaded);

		/* Register our memory allocator for mono */
		m_allocator = {MONO_ALLOCATOR_VTABLE_VERSION, settings._malloc, settings._realloc, settings._free, settings._calloc};
		mono_set_allocator_vtable(&m_allocator);

		// Create a SINGLE jit environment!
		g_jitDomain = mono_jit_init(settings.scriptSystemDomainName);

		ManagedClass::InitializeStatics();
		ManagedType::InitializeStatics();
	}

	ManagedScriptSystem::~ManagedScriptSystem()
	{
		for (auto c : m_contexts)
			delete (c);

		mono_jit_cleanup(g_jitDomain);
	}

	ManagedScriptContext* ManagedScriptSystem::CreateContext(char* domainName, const char* image)
	{
		ManagedScriptContext* ctx = new ManagedScriptContext(domainName, image);

		if (!ctx->Init())
		{
			delete ctx;
			return nullptr;
		}

		m_contexts.push_back(ctx);
		return ctx;
	}

	void ManagedScriptSystem::DestroyContext(ManagedScriptContext* ctx)
	{
		for (auto it = m_contexts.begin(); it != m_contexts.end(); ++it)
		{
			if ((*it) != ctx)
				continue;

			m_contexts.erase(it);
			delete ctx;
			return;
		}
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
		MonoProfiler* prof = &g_monoProfiler;
		printf("---- MONO PROFILE REPORT ----\n");
		printf("Total Allocations: %lu\nBytes Allocated: %lu\nTotal Moves: "
			   "%lu\nBytes Moved: %lu\n",
			   prof->totalAllocs, prof->bytesAlloc, prof->totalMoves, prof->bytesMoved);
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