#pragma once

#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <string>

#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/object.h>

struct ManagedContext;
struct ManagedAssembly;
struct ManagedScriptSettings;
struct ManagedScript;

struct ManagedAssembly
{
	MonoAssembly* m_assembly = nullptr;
	MonoImage* m_image = nullptr;
	std::string m_path;

	//std::unordered_multimap<std::string, class ManagedClass*> m_classes;

	ManagedContext* m_context = nullptr;

public:
	ManagedAssembly(ManagedContext* context, const std::string& name, MonoImage* image, MonoAssembly* assembly);
	~ManagedAssembly();

	void Close();
};

struct ManagedContext
{
	MonoDomain* m_domain = nullptr;
	std::string assemblyName;

	bool m_initialized = false;

	std::unordered_set<ManagedAssembly*> m_loadedAssemblies;

public:
	bool Initialize(MonoDomain* m_jitDomain, const char* assemblyName);
	bool LoadAssembly(const char* assemblyName);
};

struct ManagedScriptSettings
{
	std::string m_scriptDomainName;

public:
	ManagedScriptSettings(const char* domainName);

	void*	(*m_malloc)(size_t size) = malloc;
	void*	(*m_realloc)(void* mem, size_t count) = realloc;
	void	(*m_free)(void* mem) = free;
	void*	(*m_calloc)(size_t count, size_t size) = calloc;
};

struct ManagedScript
{
	MonoDomain* m_mainDomain = nullptr;

	std::unordered_set<ManagedContext*> m_contexts;

	ManagedScriptSettings	m_settings;
	MonoAllocatorVTable		m_allocators;

	static bool isInitialized;

public:
	ManagedScript(const ManagedScriptSettings& settings);
	~ManagedScript();

	ManagedContext* CreateContext(const char* image);
	void CleanUp();

};