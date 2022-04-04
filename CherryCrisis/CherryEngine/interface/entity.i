%{
	#include "entity.hpp"
%}

%include std_string.i
%include std_vector.i

%include transform_comp.i
%include camera_comp.i
%include scripted_behaviour.i

%define COMP_STD_WRAP(COMP_T)
namespace std
{
	%template(COMP_T ## Vector) vector<COMP_T*>;
};
%enddef

%define COMP_TEMPLATE_WRAP(COMP_T)
COMP_PREFIX_WRAP(Get, COMP_T, GetBehaviour)
COMP_PREFIX_WRAP(Add, COMP_T, AddBehaviour)
COMP_PREFIX_WRAP(GetAllOf, COMP_T, GetBehaviours)
%enddef

%define COMP_PREFIX_WRAP(prefix, COMP_T, funcSignature)
%csmethodmodifiers funcSignature<COMP_T> "private";
%template(prefix ## COMP_T) funcSignature<COMP_T>;
%enddef

COMP_STD_WRAP(CameraComponent)
COMP_STD_WRAP(Transform)
COMP_STD_WRAP(ScriptedBehaviour)

class Entity
{
	%csmethodmodifiers GetName() "private";

private:
	std::string m_name;
	std::vector<Behaviour*> m_behaviours;

public:
	Entity();
	Entity(const std::string& name);
	virtual ~Entity();

	template <class CompT>
	CompT* AddBehaviour();

	template <class CompT>
	CompT* GetBehaviour();

	template <class CompT>
	std::vector<CompT*> GetBehaviours();

	template <class CompT>
	bool HasBehaviour();

	template <class CompT>
	bool TryGetBehaviour(CompT*& componentToReturn);

	template <class CompT>
	CompT* GetOrAddBehaviour();

	void Destroy();

	std::string GetName();

	COMP_TEMPLATE_WRAP(CameraComponent)
	COMP_TEMPLATE_WRAP(ScriptedBehaviour)
	COMP_TEMPLATE_WRAP(Transform)

	%proxycode %{
	private Behaviour AddScript(string scriptPath)
	{
		ScriptedBehaviour script = AddScriptedBehaviour();
		script.SetScriptClass(scriptPath);

		return script.GetInstance();
	}

	private Behaviour GetScript(string scriptPath)
	{
        ScriptedBehaviourVector scriptedBehaviours = GetAllOfScriptedBehaviour();

        for (int i = 0; i < scriptedBehaviours.Count; i++)
        {
			ScriptedBehaviour script = scriptedBehaviours[i];
			if (script.GetScriptPath() == scriptPath)
				return script.GetInstance();
        }

		return null;
	}

	public string name => GetName();

	public override string ToString() => name;

	public Component AddComponent(System.Type type)
	{
		if (type == typeof(Transform))
			return AddTransform();

		if (type == typeof(CameraComponent))
			return AddCameraComponent();

		return AddScript(type.Name);
	}

	public T AddComponent<T>() where T : Component => AddComponent(typeof(T)) as T;

	public Component GetComponent(System.Type type)
	{
		if (type == typeof(Transform))
			return GetTransform();

		if (type == typeof(CameraComponent))
			return GetCameraComponent();

		return GetScript(type.Name);
	}

	public T GetComponent<T>() where T : Component => GetComponent(typeof(T)) as T;
	%}
};
