#pragma once

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

/* Mono includes */
#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/object.h>
#include <mono/metadata/profiler.h>

namespace mono
{
	class ManagedScriptSystem;
}

struct _MonoProfiler
{
	MonoProfilerHandle handle;
	uint64_t totalAllocs;
	uint64_t totalMoves;
	uint64_t bytesMoved;
	uint64_t bytesAlloc;
	mono::ManagedScriptSystem* scriptsys;
};

namespace mono
{

	template <class T>
	class ManagedBase;

	template <class T>
	class ManagedHandle
	{
	private:
		T* m_object;
		bool m_valid;

	protected:
		void Invalidate() {
			m_valid = false;
		}
		void Validate() {
			m_valid = true;
		}

		friend T;
		friend ManagedBase<T>;

	public:
		ManagedHandle(T* obj) : m_object(obj), m_valid(false)
		{
			m_object->AttachHandle(this);
		}

		virtual ~ManagedHandle()
		{
			if (m_object && m_valid)
				m_object->DetachHandle(this);
		}

		bool Valid() const { return m_valid; }

		T& operator*() { return *m_object; }

		T& operator->() { return *m_object; }
	};

	struct ManagedException_t
	{
		std::string message;
		std::string stackTrace;
		std::string source;
		std::string klass;
		std::string ns;
		std::string string_rep; // String representation of the exception (object.ToString)
	};

	//==============================================================================================//
	// ManagedBase
	//      base class for all Managed types
	//==============================================================================================//
	template <class T>
	class ManagedBase
	{
	protected:
		friend ManagedHandle<T>;

	protected:
		typedef ManagedHandle<T>* HandleT;

		HandleT m_handle;
		bool m_valid;

		ManagedBase() : m_handle(nullptr), m_valid(false) { }

		void AttachHandle(HandleT handle)
		{
			handle->Validate();
			m_handle = handle;
			if (m_valid)
				m_handle->Validate();
			else
				m_handle->Invalidate();
		}

		void DetachHandle(HandleT handle)
		{
			handle->Invalidate();
			m_handle = nullptr;
		}

		virtual void InvalidateHandle() 
	{
			if (m_handle)
				m_handle->Invalidate();
			m_valid = false;
		}

		virtual void ValidateHandle()
		{
			if (m_handle)
				m_handle->Validate();
			m_valid = true;
		}
	};

	//==============================================================================================//
	// Ref
	//      Represents a Reference to an object
	//==============================================================================================//
	template <typename T>
	using UniqueRef = std::unique_ptr<T>;

	//==============================================================================================//
	// ManagedAssembly
	//      Represents an Assembly object
	//==============================================================================================//
	class ManagedAssembly : public ManagedBase<ManagedAssembly>
	{
	private:
		typedef ManagedHandle<ManagedAssembly>* HandleT;

	private:
		MonoAssembly* m_assembly = nullptr;
		MonoImage* m_image = nullptr;
		std::string m_path;
		std::unordered_multimap<std::string, UniqueRef<class ManagedClass>> m_classes;
		bool m_populated = false;
		class ManagedScriptContext* m_ctx = nullptr;

	protected:
		friend class ManagedScriptContext;
		friend class ManagedClass;
		friend class ManagedMethod;
		friend class ManagedScriptSystem;

		void PopulateReflectionInfo();
		void DisposeReflectionInfo();

	public:
		ManagedAssembly(class ManagedScriptContext* ctx, const char* path, MonoImage* img, MonoAssembly* ass);
		virtual ~ManagedAssembly() = default;

		void GetReferencedTypes(std::vector<std::string>& refList);

		bool ValidateAgainstWhitelist(const std::vector<std::string>& whiteList);

		/* Invalidates all internal data and unloads the assembly */
		/* Delete the object after this */
		void Unload();
		void InvalidateHandle() override;

		void Reload();
		void Clear();

		inline void ReportException(MonoObject* exc);
	};

	//==============================================================================================//
	// ManagedType
	//      Represents a simple mono type
	//==============================================================================================//
	class ManagedType : public ManagedBase<ManagedType>
	{
	private:
		MonoType* m_type = nullptr;
		bool m_isStruct : 1;
		bool m_isVoid : 1;
		bool m_isRef : 1;
		bool m_isPtr : 1;
		std::string m_name;

		int m_typeindex = 0;

		static UniqueRef<ManagedType> int16Type;
		static UniqueRef<ManagedType> int32Type;
		static UniqueRef<ManagedType> int64Type;
		static UniqueRef<ManagedType> uint16Type;
		static UniqueRef<ManagedType> uint32Type;
		static UniqueRef<ManagedType> uint64Type;
		static UniqueRef<ManagedType> stringType;
		static UniqueRef<ManagedType> booleanType;
		static UniqueRef<ManagedType> threadType;
		static UniqueRef<ManagedType> singleType;
		static UniqueRef<ManagedType> byteType;
		static UniqueRef<ManagedType> arrayType;
		static UniqueRef<ManagedType> charType;
		static UniqueRef<ManagedType> doubleType;
		static UniqueRef<ManagedType> enumType;
		static UniqueRef<ManagedType> exceptionType;
		static UniqueRef<ManagedType> intptrType;
		static UniqueRef<ManagedType> uintptrType;
		static UniqueRef<ManagedType> sbyteType;
		static UniqueRef<ManagedType> voidType;

		static void InitializeStatics();

	protected:
		friend class ManagedMethod;
		friend class ManagedScriptSystem;
		friend class ManagedObject;

	public:
		ManagedType(MonoType* type);

		void Reload();

		bool IsStruct() const { return m_isStruct; }
		bool IsVoid()	const { return m_isVoid; }
		bool IsRef()	const { return m_isRef; }
		bool IsPtr()	const { return m_isPtr; }

		bool Equals(const ManagedType* other) const;
		bool operator==(const ManagedType* other) const { return Equals(other); }

		const std::string& Name();
		int TypeIndex() const { return m_typeindex; }

		inline MonoType* RawType() const { return m_type; }

		inline static const ManagedType* GetInt16()		{ return int16Type.get(); }
		inline static const ManagedType* GetInt32()		{ return int32Type.get(); }
		inline static const ManagedType* GetInt64()		{ return int64Type.get(); }
		inline static const ManagedType* GetUint16()	{ return uint16Type.get(); }
		inline static const ManagedType* GetUint32()	{ return uint32Type.get(); }
		inline static const ManagedType* GetUint64()	{ return uint64Type.get(); }
		inline static const ManagedType* GetString()	{ return stringType.get(); }
		inline static const ManagedType* GetBoolean()	{ return booleanType.get(); }
		inline static const ManagedType* GetThread()	{ return threadType.get(); }
		inline static const ManagedType* GetSingle()	{ return singleType.get(); }
		inline static const ManagedType* GetByte()		{ return byteType.get(); }
		inline static const ManagedType* GetArray()		{ return arrayType.get(); }
		inline static const ManagedType* GetChar()		{ return charType.get(); }
		inline static const ManagedType* GetDouble()	{ return doubleType.get(); }
		inline static const ManagedType* GetEnum()		{ return enumType.get(); }
		inline static const ManagedType* GetException()	{ return exceptionType.get(); }
		inline static const ManagedType* GetIntptr()	{ return intptrType.get(); }
		inline static const ManagedType* GetUintptr()	{ return uintptrType.get(); }
		inline static const ManagedType* GetSbyte()		{ return sbyteType.get(); }
		inline static const ManagedType* GetVoid()		{ return voidType.get(); }
	};

	//==============================================================================================//
	// ManagedObjectType
	//		The type of managed object it should be
	//==============================================================================================//
	enum class EManagedObjectHandleType
	{
		/**
		 * Generic default handle type. Backed by mono_gchandle_new, but is not
		 * pinned Since the address may change, accesses require calls into the mono
		 * api which may incur overhead
		 */
		HANDLE = 0,
		/**
		 * Generic default handle type, but pinned. Backed by mono_gchandle_new, but
		 * pinned. Requires no calls into the mono api for accesses, so no overhead
		 * for accesses or modifications
		 */
		HANDLE_PINNED = 1,
		/**
		 * Weak reference handle type. Objects pointed to by weak handles may have
		 * their memory reclaimed by the GC. As such, mono api calls are required to
		 * obtain the actual object's address on access
		 */
		WEAKREF = 2,
	};

	//==============================================================================================//
	// ManagedObject
	//      Wrapper around a mono object
	//      Unlike the other classes here, the managed object can be copied around.
	//      It's just a wrapper around a MonoObject.
	//==============================================================================================//
	using ManagedObjectHandle = uint32_t;

	class ManagedObject : public ManagedBase<ManagedObject>
	{
	private:
		MonoObject* m_obj = nullptr;
		class ManagedClass* m_class = nullptr;
		uint32_t m_gcHandle = 0;
		EManagedObjectHandleType m_handleType = EManagedObjectHandleType::HANDLE_PINNED;
		void* m_handledPtr = nullptr;

		std::function<MonoObject*()> m_getObject;

		friend class ManagedClass;
		friend class ManagedMethod;
		friend class ManagedScriptContext;

	public:
		ManagedObject(MonoObject* obj, class ManagedClass& cls, EManagedObjectHandleType type = EManagedObjectHandleType::HANDLE_PINNED);
		~ManagedObject();

		const ManagedClass* Class() const { return m_class; }

		const MonoObject* RawObject() const { return m_getObject(); }
		MonoObject* RawObject() { return m_getObject(); }

		ManagedObjectHandle GCHandle() { return m_gcHandle; }

		EManagedObjectHandleType GCHandleType() { return m_handleType; }

		bool SetProperty(class ManagedProperty* prop, const void* value);
		bool SetField(class ManagedField* field, const void* value);
		bool GetProperty(class ManagedProperty* prop, void** outValue);
		bool GetField(class ManagedField* field, void* outValue);

		bool SetProperty(const char* p, const void* value);
		bool SetField(const char* p, const void* value);
		bool GetProperty(const char* p, void** outValue);
		bool GetField(const char* p, void* outValue);

		void Dispose();
		void Reload();

		MonoObject* Invoke(class ManagedMethod* method, void** params);
	};

	class AManagedThunk : public ManagedBase<AManagedThunk>
	{
	protected:
		MonoMethod* m_method = nullptr;

		friend class ManagedMethod;

	public:
		virtual void Reload() = 0;
	};

	template <typename RetT, typename ...Args>
	class ManagedThunk : public AManagedThunk
	{
	private:
		std::function<RetT(Args..., MonoException**)> m_unmanagedThunk;

	public:
		ManagedThunk() = default;
		ManagedThunk(void* func)
			: ManagedThunk(std::function<RetT(Args..., MonoException**)>((RetT(*)(Args..., MonoException**))func)) { }
		ManagedThunk(const std::function<RetT(Args..., MonoException**)>& func)
			: m_unmanagedThunk(func) {}

		template <typename ...Params>
		RetT Invoke(Params&&... params) { return m_unmanagedThunk(params...); }

		void Reload() override
		{
			m_unmanagedThunk = (RetT(*)(Args..., MonoException**))mono_method_get_unmanaged_thunk(m_method);
		}
	};

	//==============================================================================================//
	// ManagedMethod
	//      Represents a MonoMethod object, must be a part of a class
	//==============================================================================================//
	class ManagedMethod : public ManagedBase<ManagedMethod>
	{
	private:
		MonoMethod* m_method = nullptr;
		class ManagedClass* m_class = nullptr;
		std::vector<class ManagedObject*> m_attributes;
		MonoCustomAttrInfo* m_attrInfo = nullptr;
		MonoMethodSignature* m_signature = nullptr;
		bool m_populated = false;
		uint32_t m_token = 0;
		std::string m_name;
		std::string m_fullyQualifiedName;
		int m_paramCount = 0;

		UniqueRef<ManagedType> m_returnType = nullptr;
		std::vector<UniqueRef<ManagedType>> m_params;
		std::vector<UniqueRef<AManagedThunk>> m_thunks;

		friend class ManagedClass;
		friend ManagedHandle<ManagedMethod>;

	protected:
		friend class ExecutionContext;
		friend class ManagedClass;
		friend class ManagedObject;

		void InvalidateHandle() override;

	public:
		ManagedMethod(MonoMethod* method, ManagedClass* cls);
		virtual ~ManagedMethod();

		template <typename RetT, typename ...Args>
		ManagedThunk<RetT, MonoObject*, Args...>* GetMemberUnmanagedThunk()
		{
			void* func = mono_method_get_unmanaged_thunk(m_method);

			auto thunkPtr = new ManagedThunk<RetT, MonoObject*, Args...>(func);
			m_thunks.emplace_back(thunkPtr);

			return thunkPtr;
		}

		template <typename RetT, typename ...Args>
		ManagedThunk<RetT, Args...>* GetStaticUnmanagedThunk()
		{
			void* func = mono_method_get_unmanaged_thunk(m_method);

			auto thunkPtr = new ManagedThunk<RetT, Args...>(func);
			m_thunks.emplace_back(thunkPtr);

			return thunkPtr;
		}

		ManagedAssembly& Assembly() const;

		ManagedClass* Class() const { return m_class; }

		const std::vector<ManagedObject*>& Attributes() const { return m_attributes; }

		const std::string& Name() const { return m_name; }

		int ParamCount() const { return m_paramCount; }

		MonoMethod* RawMethod() { return m_method; }

		bool MatchSignature(MonoMethod* method);
		bool MatchSignature(MonoType* returnval, std::vector<MonoType*> params);
		bool MatchSignature(std::vector<MonoType*> params);
		bool MatchSignature();

		bool Reload();

		MonoObject* Invoke(ManagedObject* obj, void** params, MonoObject** exception = nullptr);
		MonoObject* InvokeStatic(void** params, MonoObject** exception = nullptr);
	};

	//==============================================================================================//
	// ManagedField
	//      Represents a MonoField, or a field in a class
	//==============================================================================================//
	class ManagedField : public ManagedBase<ManagedField>
	{
	private:
		MonoClassField* m_field;
		ManagedType m_type = nullptr;
		class ManagedClass* m_class;
		std::string m_name;

	public:
		inline ManagedType* Type() { return &m_type; }

		inline ManagedClass* Class() const { return m_class; }

		inline MonoClassField* RawField() const { return m_field; }

		const std::string& Name() const { return m_name; }

	protected:
		friend class ManagedClass;
		friend class ManagedProperty;
		friend class ManagedObject;

	public:
		ManagedField(MonoClassField* fld, class ManagedClass* cls, const char* name);
		virtual ~ManagedField() = default;
	};

	//==============================================================================================//
	// ManagedProperty
	//      Represents a MonoProperty
	//==============================================================================================//
	class ManagedProperty : public ManagedBase<ManagedProperty>
	{
	private:
		MonoProperty* m_property = nullptr;
		class ManagedClass* m_class;
		std::string m_name;
		MonoMethod* m_getMethod = nullptr;
		MonoMethod* m_setMethod = nullptr;

	protected:
		friend class ManagedClass;
		friend class ManagedMethod;
		friend class ManagedObject;

	public:
		ManagedProperty(MonoProperty* prop, ManagedClass* cls, const char* name);
		virtual ~ManagedProperty() = default;

		const MonoProperty* RawProperty() const { return m_property; }

		const ManagedClass* Class() const { return m_class; }
	};

	//==============================================================================================//
	// ManagedClass
	//      Represents a MonoClass object and stores cached info about it
	//==============================================================================================//
	class ManagedClass : public ManagedBase<ManagedClass>
	{
	private:
		std::unordered_multimap<std::string, UniqueRef<class ManagedMethod>> m_methods;
		std::unordered_map<std::string, UniqueRef<class ManagedField>> m_fields;
		std::unordered_map<std::string, UniqueRef<class ManagedProperty>> m_properties;
		std::vector<UniqueRef<class ManagedObject>> m_attributes;
		std::vector<UniqueRef<class ManagedProperty>> m_instances;
		std::vector<UniqueRef<class ManagedObject>> m_handledInstances;
		std::vector<UniqueRef<class ManagedObject>> m_managedInstances;
		MonoCustomAttrInfo* m_attrInfo;
		std::string m_namespaceName;
		std::string m_className;
		MonoClass* m_class;
		ManagedAssembly* m_assembly;
		mono_byte m_numConstructors = 0;
		mono_byte m_alignment;

		bool m_populated : 1 = false;
		bool m_valueClass : 1;
		bool m_delegateClass : 1;
		bool m_enumClass : 1;
		bool m_nullableClass : 1;

		uint32_t m_size; // Size in bytes

		ManagedMethod* GetCtor(std::vector<MonoType*> signature);

		static UniqueRef<ManagedClass> int16Class;
		static UniqueRef<ManagedClass> int32Class;
		static UniqueRef<ManagedClass> int64Class;
		static UniqueRef<ManagedClass> uint16Class;
		static UniqueRef<ManagedClass> uint32Class;
		static UniqueRef<ManagedClass> uint64Class;
		static UniqueRef<ManagedClass> stringClass;
		static UniqueRef<ManagedClass> booleanClass;
		static UniqueRef<ManagedClass> threadClass;
		static UniqueRef<ManagedClass> singleClass;
		static UniqueRef<ManagedClass> byteClass;
		static UniqueRef<ManagedClass> arrayClass;
		static UniqueRef<ManagedClass> charClass;
		static UniqueRef<ManagedClass> doubleClass;
		static UniqueRef<ManagedClass> enumClass;
		static UniqueRef<ManagedClass> exceptionClass;
		static UniqueRef<ManagedClass> intptrClass;
		static UniqueRef<ManagedClass> uintptrClass;
		static UniqueRef<ManagedClass> sbyteClass;
		static UniqueRef<ManagedClass> voidClass;

		static void InitializeStatics();

		friend class ManagedScriptContext;
		friend class ManagedScriptSystem;
		friend class ManagedMethod;
		friend class ManagedAssembly;
		friend class ManagedObject;
	protected:
		void PopulateReflectionInfo();

		void InvalidateHandle() override;

	public:
		ManagedClass(MonoClass* cls);
		ManagedClass(ManagedAssembly* assembly, MonoClass* cls);
		virtual ~ManagedClass();

		void Reload();
		void Clear();

		const std::string& NamespaceName() const { return m_namespaceName; }
		const std::string& ClassName() const { return m_className; }

		const std::unordered_multimap<std::string, UniqueRef<class ManagedMethod>>& Methods() const { return m_methods; }
		const std::unordered_map<std::string, UniqueRef<ManagedField>>& Fields() const { return m_fields; }
		const std::unordered_map<std::string, UniqueRef<ManagedProperty>>& Properties() const { return m_properties; }
		const std::vector<UniqueRef<ManagedObject>>& Attributes() const { return m_attributes; }

		uint32_t DataSize() const { return m_size; }
		bool ValueClass() const { return m_valueClass; }
		bool DelegateClass() const { return m_delegateClass; }
		bool EnumClass() const { return m_enumClass; }
		bool Nullable() const { return m_nullableClass;	}
		int Alignment() const {	return m_alignment;	}

		mono_byte NumConstructors() const;

		ManagedMethod* FindMethod(const char* name);
		ManagedField* FindField(const char* name);
		ManagedProperty* FindProperty(const char* prop);

		MonoObject* CreateRawInstance(std::vector<MonoType*> signature, void** params);
		ManagedObject* CreateInstance(std::vector<MonoType*> signature, void** params);
		MonoObject* CreateUnmanagedRawInstance(void* cPtr, bool ownMemory);
		ManagedObject* CreateUnmanagedInstance(void* cPtr, bool ownMemory);

		inline MonoType* RawType() const { return mono_class_get_type(m_class);	}

		bool ImplementsInterface(ManagedClass& interface);
		bool DerivedFromClass(ManagedClass& cls);
		bool DerivedFromClass(MonoClass& cls);

		inline bool IsVoid();
		inline bool IsInt16();
		inline bool IsInt32();
		inline bool IsInt64();
		inline bool IsDouble();
		inline bool IsIntptr();
		inline bool IsThread();
		inline bool IsArray();
		inline bool IsByte();
		inline bool IsChar();
		inline bool IsUInt32();
		inline bool IsUInt16();
		inline bool IsUInt64();
		inline bool IsUIntptr();
		inline bool IsBool();

		inline static const ManagedClass* GetInt16()		{ return int16Class.get(); }
		inline static const ManagedClass* GetInt32()		{ return int32Class.get(); }
		inline static const ManagedClass* GetInt64()		{ return int64Class.get(); }
		inline static const ManagedClass* GetUint16()		{ return uint16Class.get(); } 
		inline static const ManagedClass* GetUint32()		{ return uint32Class.get(); } 
		inline static const ManagedClass* GetUint64()		{ return uint64Class.get(); } 
		inline static const ManagedClass* GetString()		{ return stringClass.get(); } 
		inline static const ManagedClass* GetBoolean()		{ return booleanClass.get(); } 
		inline static const ManagedClass* GetThread()		{ return threadClass.get(); } 
		inline static const ManagedClass* GetSingle()		{ return singleClass.get(); } 
		inline static const ManagedClass* GetByte()			{ return byteClass.get(); } 
		inline static const ManagedClass* GetArray()		{ return arrayClass.get(); } 
		inline static const ManagedClass* GetChar()			{ return charClass.get(); } 
		inline static const ManagedClass* GetDouble()		{ return doubleClass.get(); } 
		inline static const ManagedClass* GetEnum()			{ return enumClass.get(); } 
		inline static const ManagedClass* GetException()	{ return exceptionClass.get(); } 
		inline static const ManagedClass* GetIntptr()		{ return intptrClass.get(); } 
		inline static const ManagedClass* GetUintptr()		{ return uintptrClass.get(); } 
		inline static const ManagedClass* GetSbyte()		{ return sbyteClass.get(); } 
		inline static const ManagedClass* GetVoid()			{ return voidClass.get(); } 
	};

	/* NOTE: this class cannot have a handle pointed at it */
	//==============================================================================================//
	// ManagedScriptContext
	//      Handles execution of a "script"
	//==============================================================================================//
	class ManagedScriptContext
	{
	public:
		std::list<UniqueRef<ManagedAssembly>> m_loadedAssemblies;
		std::string m_domainName;
		std::string m_path;
		MonoDomain* m_domain = nullptr;
		std::string m_baseImage;

	public:
		friend class ManagedCompiler;
		friend class ManagedClass;

		using ExceptionCallbackT = std::function<void(ManagedScriptContext*, ManagedAssembly*, MonoObject*, ManagedException_t)>;

	protected:
		std::vector<ExceptionCallbackT> m_callbacks;

		friend class ManagedScriptSystem;

		void PopulateReflectionInfo();

	public:
		ManagedScriptContext(char* domainName, const char* path, const char* baseImage);
		virtual ~ManagedScriptContext();

		const std::string& GetImagePath() { return m_baseImage; }
		const std::string& GetPath() { return m_path; }

		bool Reload();

		bool LoadAssembly(const char* path);

		bool UnloadAssembly(const char* name);

		bool Init();

		bool Unload();

		/* Performs a class search in all loaded assemblies */
		/* If you have the assembly name, please use the alternative version of this
		 * function */
		ManagedClass* FindClass(const char* ns, const char* cls);

		ManagedClass* FindClass(ManagedAssembly* assembly, const char* ns, const char* cls);

		/* Returns a pointer to a raw MonoClass object corresponding to the
		 * specified class */
		/* This doesn't cache the class in a lookup table. You'll need to save the
		 * class yourself */
		MonoClass* FindSystemClass(const char*, const char* cls);

		ManagedAssembly* FindAssembly(const char* path);

		ManagedException_t GetExceptionDescriptor(MonoObject* exception);

		/* Clears all reflection info stored in each assembly description */
		/* WARNING: this will invalidate your handles! */
		void ClearReflectionInfo();

		bool ValidateAgainstWhitelist(const std::vector<std::string>& whitelist);

		void ReportException(MonoObject* obj, ManagedAssembly& ass);

		void RegisterExceptionCallback(ExceptionCallbackT callback)
		{
			m_callbacks.push_back(callback);
		}

		MonoDomain* RawDomain() const { return m_domain; }
	};

	//==============================================================================================//
	// ManagedScriptSystem
	//      Handles execution of a "script"
	//==============================================================================================//
	struct ManagedScriptSystemSettings_t
	{
		/* Readable name of the domain to be created */
		const char* scriptSystemDomainName;

		/* if true, configData is a file path that will be parsed by mono */
		bool configIsFile = false;
		/* If configIsFile is true, this is a file path to the config. Otherwise,
		 * this is raw text data from the cfg file */
		const char* configData = nullptr;

		/* Overrides for the default mono allocators */
		void*	(*_malloc)(size_t size) = malloc;
		void*	(*_realloc)(void* mem, size_t count) = realloc;
		void	(*_free)(void* mem) = free;
		void*	(*_calloc)(size_t count, size_t size) = calloc;

		ManagedScriptSystemSettings_t(const char* domainName)
			: scriptSystemDomainName(domainName) { }
	};

	struct ManagedProfilingData_t
	{
		size_t bytesMoved;	// How many bytes have been moved in total
		size_t totalMoves;	// Individual move operations
		size_t bytesAlloc;	// Number of bytes allocated
		size_t totalAllocs; // Number of allocation operations
		size_t totalContextUnloads;
		size_t totalContextLoads;
	};

	struct ManagedProfilingSettings_t
	{
		/* Profiling enable */
		bool enableProfiling : 1;
		/* Things to profile */
		bool profileCalls : 1;
		bool profileCoverage : 1;
		bool profileAllocations : 1;
		bool profileDomain : 1;
		bool profileContext : 1;
		bool profileAssembly : 1;
		bool profileImage : 1;
		bool profileExceptions : 1;
		bool profileGC : 1;
		bool profileThread : 1;		 /* Profile threading events */
		bool recordThreadEvents : 1; /* Log thread start/stop events in a
										timestamped log */
	};

	class ManagedScriptSystem
	{
	private:
		MonoProfiler m_monoProfiler;
		MonoDomain* m_rootDomain;

		std::unordered_map<std::string, UniqueRef<ManagedScriptContext>> m_contexts;
		std::stack<ManagedProfilingData_t> m_profilingData;
		MonoAllocatorVTable m_allocator;
		ManagedScriptSystemSettings_t m_settings;
		ManagedProfilingData_t* m_curFrame = nullptr;
		bool m_debugEnabled;
		ManagedProfilingSettings_t m_profilingSettings;

	public:
		ManagedScriptSystem(ManagedScriptSystemSettings_t settings);
		virtual ~ManagedScriptSystem();

		/* NO COPIES! */
		ManagedScriptSystem(ManagedScriptSystem&) = delete;
		ManagedScriptSystem(ManagedScriptSystem&&) = delete;
		ManagedScriptSystem() = delete;

		ManagedScriptContext* CreateContext(char* domainName, const char* path, const char* image);

		std::unordered_map<std::string, UniqueRef<ManagedScriptContext>>& GetContextes() { return m_contexts; }
			
		void DestroyContext(ManagedScriptContext* ctx);

		size_t NumActiveContexts() const { return m_contexts.size(); }

		uint64_t HeapSize() const;

		uint64_t UsedHeapSize() const;

		void RegisterNativeFunction(const char* name, void* func);

		void ReportProfileStats();

		bool IsDebuggingEnabled() const { return m_debugEnabled; }

		ManagedProfilingSettings_t GetProfilingSettings() { return m_profilingSettings;	}
		void SetProfilingSettings(ManagedProfilingSettings_t settings);

		uint32_t MaxGCGeneration();
		void RunGCCollect(uint32_t gen);
		void RunGCCollectAll();

		void PushProfilingContext();
		void PopProfilingContext();
		inline ManagedProfilingData_t& CurrentProfilingData() { return *m_curFrame; };// This needs to be fast
	};

}