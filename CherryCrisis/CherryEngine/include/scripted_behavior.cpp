#include "pch.hpp"

#include "scripted_behaviour.hpp"

ScriptedBehaviour::ScriptedBehaviour(Entity& owner)
	: Behaviour(owner)
{
	// TODO: Move this in the proper files
	mono_set_dirs(".\\lib", ".\\externals\\etc");

	/*static const char* options[] = {
		  "--soft-breakpoints",
		  "--debugger-agent=transport=dt_socket,address=127.0.0.1:55555"
	};

	mono_jit_parse_options(sizeof(options) / sizeof(char*), (char**)options);
	mono_debug_init(MONO_DEBUG_FORMAT_MONO);*/

	settings = std::make_shared<ManagedScriptSettings>("CsProject");

	script = std::make_shared<ManagedScript>(*settings);

	context = script->CreateContext("../x64/Debug/CherryScripting.dll");

	behaviourClass = mono_class_from_name((*context->m_loadedAssemblies.begin())->m_image, "CherryScripting", "MyComponent");

	MonoClass* entityClass = mono_class_from_name((*context->m_loadedAssemblies.begin())->m_image, "CCEngine", "Entity");
	MonoObject* entityInst = mono_object_new(context->m_domain, entityClass);

	behaviourInst = mono_object_new(context->m_domain, behaviourClass);

	MonoMethod* currMethod;
	void* iter = nullptr;
	while (currMethod = mono_class_get_methods(behaviourClass, &iter)) {
		if (strcmp(mono_method_get_name(currMethod), ".ctor") == 0) {
			ctor = currMethod;
			break;
		}
	}

	MonoMethod* entCtor = nullptr;
	iter = currMethod = nullptr;
	while (currMethod = mono_class_get_methods(entityClass, &iter)) {
		if (strcmp(mono_method_get_name(currMethod), ".ctor") == 0) {
			entCtor = currMethod;
		}
	}
	bool memOwn = true;
	void* entArgs[2] = { &GetHost(), &memOwn };
	mono_runtime_invoke(entCtor, entityInst, entArgs, nullptr);


	int num = 50;
	void* args[2] = { &num, entityInst };
	mono_runtime_invoke(ctor, behaviourInst, args, nullptr);


	updateDesc = mono_method_desc_new(":Update()", false);
	updateMethod = mono_method_desc_search_in_class(updateDesc, behaviourClass);
}

void ScriptedBehaviour::SetupInterface()
{

}

void ScriptedBehaviour::Start()
{
	SetupInterface();
}

void ScriptedBehaviour::Update()
{
	mono_runtime_invoke(updateMethod, behaviourInst, nullptr, nullptr);
}