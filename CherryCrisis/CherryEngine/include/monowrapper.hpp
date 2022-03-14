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
	using Ref = std::shared_ptr<T>;

	template <typename T>
	using WeakRef = std::shared_ptr<T>;

	//==============================================================================================//
	// ManagedAssembly
	//      Represents an Assembly object
	//==============================================================================================//
	class ManagedAssembly : public ManagedBase<ManagedAssembly>
	{
	private:
		typedef ManagedHandle<ManagedAssembly>* HandleT;

	private:
		MonoAssembly* m_assembly;
		MonoImage* m_image;
		std::string m_path;
		std::unordered_multimap<std::string, Ref<class ManagedClass>> m_classes;
		bool m_populated;
		class ManagedScriptContext* m_ctx;

	protected:
		friend class ManagedScriptContext;
		friend class ManagedClass;
		friend class ManagedMethod;

		void PopulateReflectionInfo();
		void DisposeReflectionInfo();

	public:
		ManagedAssembly(class ManagedScriptContext* ctx, const std::string& path, MonoImage* img, MonoAssembly* ass);
		virtual ~ManagedAssembly() = default;

		void GetReferencedTypes(std::vector<std::string>& refList);

		bool ValidateAgainstWhitelist(const std::vector<std::string>& whiteList);

		/* Invalidates all internal data and unloads the assembly */
		/* Delete the object after this */
		void Unload();
		void InvalidateHandle() override;

		void Reload();

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

	protected:
		ManagedType(MonoType* type);

		friend class ManagedMethod;
		friend class ManagedObject;

	public:
		bool IsStruct() const { return m_isStruct; }
		bool IsVoid() const { return m_isVoid; }
		bool IsRef() const { return m_isRef; }
		bool IsPtr() const { return m_isPtr; }

		bool Equals(const ManagedType* other) const;

		const std::string& Name() const;

		inline MonoType* RawType() const { return m_type; }
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

		std::function<MonoObject*()> m_getObject;

		friend class ManagedClass;
		friend class ManagedMethod;
		friend class ManagedScriptContext;

	public:
		ManagedObject(MonoObject* obj, class ManagedClass& cls, EManagedObjectHandleType type = EManagedObjectHandleType::HANDLE_PINNED);
		~ManagedObject();

		const ManagedClass& Class() const { return *m_class; }

		const MonoObject* RawObject() const { return m_getObject(); }
		MonoObject* RawObject() { return m_getObject(); }

		ManagedObjectHandle GCHandle() { return m_gcHandle; }

		EManagedObjectHandleType GCHandleType() { return m_handleType; }

		bool SetProperty(class ManagedProperty& prop, void* value);
		bool SetField(class ManagedField& prop, void* value);
		bool GetProperty(class ManagedProperty& prop, void** outValue);
		bool GetField(class ManagedField& prop, void* outValue);

		bool SetProperty(const std::string& p, void* value);
		bool SetField(const std::string& p, void* value);
		bool GetProperty(const std::string& p, void** outValue);
		bool GetField(const std::string& p, void* outValue);

		MonoObject* Invoke(class ManagedMethod* method, void** params);
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
		bool m_populated;
		uint32_t m_token;
		std::string m_name;
		std::string m_fullyQualifiedName;
		int m_paramCount;

		ManagedType* m_returnType = nullptr;
		std::vector<ManagedType*> m_params;

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

		ManagedAssembly& Assembly() const;

		ManagedClass& Class() const;

		const std::vector<ManagedObject*>& Attributes() const { return m_attributes; }

		const std::string& Name() const { return m_name; }

		int ParamCount() const { return m_paramCount; }

		MonoMethod* RawMethod() { return m_method; }

		bool MatchSignature(MonoType* returnval, std::vector<MonoType*> params);
		bool MatchSignature(std::vector<MonoType*> params);
		bool MatchSignature();

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
		MonoType* m_type = nullptr;
		class ManagedClass& m_class;
		std::string m_name;

	public:
		inline ManagedClass& Class() const { return m_class; }

		inline MonoClassField* RawField() const { return m_field; }

		const std::string& Name() const { return m_name; }

	protected:
		friend class ManagedClass;
		friend class ManagedProperty;
		friend class ManagedObject;

	public:
		ManagedField(MonoClassField* fld, class ManagedClass& cls, const char* name);
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
		class ManagedClass& m_class;
		std::string m_name;
		MonoMethod* m_getMethod = nullptr;
		MonoMethod* m_setMethod = nullptr;

	protected:
		friend class ManagedClass;
		friend class ManagedMethod;
		friend class ManagedObject;

	public:
		ManagedProperty(MonoProperty& prop, ManagedClass& cls, const char* name);
		virtual ~ManagedProperty() = default;

		const MonoProperty* RawProperty() const { return m_property; }

		const ManagedClass& Class() const { return m_class; }
	};

	//==============================================================================================//
	// ManagedClass
	//      Represents a MonoClass object and stores cached info about it
	//==============================================================================================//
	class ManagedClass : public ManagedBase<ManagedClass>
	{
	private:
		std::vector<Ref<class ManagedMethod>> m_methods;
		std::unordered_map<std::string, Ref<class ManagedField>> m_fields;
		std::unordered_map<std::string, Ref<class ManagedProperty>> m_properties;
		std::vector<Ref<class ManagedObject>> m_attributes;
		std::vector<Ref<class ManagedProperty>> m_instances;
		MonoCustomAttrInfo* m_attrInfo;
		std::string m_namespaceName;
		std::string m_className;
		MonoClass* m_class;
		ManagedAssembly* m_assembly;
		mono_byte m_numConstructors;
		mono_byte m_alignment;

		bool m_populated : 1;
		bool m_valueClass : 1;
		bool m_delegateClass : 1;
		bool m_enumClass : 1;
		bool m_nullableClass : 1;

		uint32_t m_size; // Size in bytes

		friend class ManagedScriptContext;
		friend class ManagedMethod;
		friend class ManagedAssembly;
		friend class ManagedObject;

	protected:
		void PopulateReflectionInfo();

		void InvalidateHandle() override;

	public:
		ManagedClass(ManagedAssembly* assembly, const std::string& ns, const std::string& cls);
		ManagedClass(ManagedAssembly* assembly, MonoClass* _cls, const std::string& ns, const std::string& cls);
		virtual ~ManagedClass();

		void Reload();

		const std::string& NamespaceName() const { return m_namespaceName; }
		const std::string& ClassName() const { return m_className; }

		const std::vector<Ref<ManagedMethod>>& Methods() const { return m_methods; }
		const std::unordered_map<std::string, Ref<ManagedField>>& Fields() const { return m_fields; }
		const std::unordered_map< std::string, Ref<ManagedProperty>>& Properties() const { return m_properties; }
		const std::vector<Ref<ManagedObject>>& Attributes() const { return m_attributes; }

		uint32_t DataSize() const { return m_size; }
		bool ValueClass() const { return m_valueClass; }
		bool DelegateClass() const { return m_delegateClass; }
		bool EnumClass() const { return m_enumClass; }
		bool Nullable() const { return m_nullableClass;	}
		int Alignment() const {	return m_alignment;	}

		mono_byte NumConstructors() const;

		Ref<ManagedMethod> FindMethod(const std::string& name);
		Ref<ManagedField> FindField(const std::string& name);
		Ref<ManagedProperty> FindProperty(const std::string& prop);

		Ref<ManagedObject> CreateInstance(std::vector<MonoType*> signature, void** params);

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
	};

	/* NOTE: this class cannot have a handle pointed at it */
	//==============================================================================================//
	// ManagedScriptContext
	//      Handles execution of a "script"
	//==============================================================================================//
	class ManagedScriptContext
	{
	public:
		std::list<Ref<ManagedAssembly>> m_loadedAssemblies;
		std::string m_domainName;
		MonoDomain* m_domain = nullptr;
		std::string m_baseImage;
		bool m_initialized = false;

	public:
		friend class ManagedCompiler;
		friend class ManagedClass;

		using ExceptionCallbackT =
			std::function<void(ManagedScriptContext*, ManagedAssembly*, MonoObject*, ManagedException_t)>;

	protected:
		std::vector<ExceptionCallbackT> m_callbacks;

		friend class ManagedScriptSystem;

		explicit ManagedScriptContext(char* domainName, const std::string& baseImage);
		~ManagedScriptContext();

		void PopulateReflectionInfo();

	public:
		bool Reload();

		bool LoadAssembly(const char* path);

		bool UnloadAssembly(const std::string& name);

		bool Init();

		bool Unload();

		/* Performs a class search in all loaded assemblies */
		/* If you have the assembly name, please use the alternative version of this
		 * function */
		Ref<ManagedClass> FindClass(const std::string& ns, const std::string& cls);

		Ref<ManagedClass> FindClass(ManagedAssembly& assembly, const std::string& ns, const std::string& cls);

		/* Returns a pointer to a raw MonoClass object corresponding to the
		 * specified class */
		/* This doesn't cache the class in a lookup table. You'll need to save the
		 * class yourself */
		MonoClass* FindSystemClass(const std::string& ns, const std::string& cls);

		Ref<ManagedAssembly> FindAssembly(const std::string& path);

		ManagedException_t GetExceptionDescriptor(MonoObject* exception);

		/* Clears all reflection info stored in each assembly description */
		/* WARNING: this will invalidate your handles! */
		void ClearReflectionInfo();

		bool ValidateAgainstWhitelist(const std::vector<std::string>& whitelist);

		void ReportException(MonoObject& obj, ManagedAssembly& ass);

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
		std::vector<ManagedScriptContext*> m_contexts;
		std::stack<ManagedProfilingData_t> m_profilingData;
		MonoAllocatorVTable m_allocator;
		ManagedScriptSystemSettings_t m_settings;
		ManagedProfilingData_t* m_curFrame = nullptr;;
		bool m_debugEnabled;
		ManagedProfilingSettings_t m_profilingSettings;

	public:
		explicit ManagedScriptSystem(ManagedScriptSystemSettings_t settings);
		~ManagedScriptSystem();

		/* NO COPIES! */
		ManagedScriptSystem(ManagedScriptSystem&) = delete;
		ManagedScriptSystem(ManagedScriptSystem&&) = delete;
		ManagedScriptSystem() = delete;

		ManagedScriptContext* CreateContext(char* domainName, const char* image);

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