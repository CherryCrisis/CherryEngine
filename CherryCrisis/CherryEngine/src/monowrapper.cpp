#include "pch.hpp"

#include "monowrapper.hpp"

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

bool ManagedScript::isInitialized = false;

ManagedAssembly::ManagedAssembly(ManagedContext* context, const std::string& name, MonoImage* image, MonoAssembly* assembly)
	: m_context(context), m_path(name), m_image(image), m_assembly(assembly)
{

}

ManagedAssembly::~ManagedAssembly()
{
	Close();
}

void ManagedAssembly::Close()
{
	mono_assembly_close(m_assembly);
}

ManagedScriptSettings::ManagedScriptSettings(const char* domainName)
	: m_scriptDomainName(domainName)
{

}

bool ManagedContext::Initialize(MonoDomain* m_jitDomain, const char* assemblyName)
{
	m_domain = m_jitDomain;

	if (!LoadAssembly(assemblyName))
		return false;

	m_initialized = true;
	return true;
}

bool ManagedContext::LoadAssembly(const char* assemblyName)
{
	if (!m_domain)
		return false;
	
	MonoAssembly* assembly = mono_domain_assembly_open(m_domain, assemblyName);
	if (!assembly)
		return false;

	MonoImage* image = mono_assembly_get_image(assembly);
	
	if (!image)
		return false;

	ManagedAssembly* managedAssembly = new ManagedAssembly(this, assemblyName, image, assembly);
	m_loadedAssemblies.insert(managedAssembly);
	return true;
}

ManagedScript::ManagedScript(const ManagedScriptSettings& settings)
	: m_settings(settings)
{
	if (isInitialized)
		abort();

	m_allocators =  {
						MONO_ALLOCATOR_VTABLE_VERSION,
						settings.m_malloc,
						settings.m_realloc,
						settings.m_free,
						settings.m_calloc,
					};

	mono_set_allocator_vtable(&m_allocators);

	m_mainDomain = mono_jit_init("jit");
}

ManagedContext* ManagedScript::CreateContext(const char* image)
{
	auto context = new ManagedContext();

	if (!context->Initialize(m_mainDomain, image))
		return nullptr;

	m_contexts.insert(context);
	return context;
}

void ManagedScript::CleanUp()
{
	if (m_mainDomain)
	{
		mono_jit_cleanup(m_mainDomain);
		m_mainDomain = nullptr;
	}
}


ManagedScript::~ManagedScript()
{
	CleanUp();
}