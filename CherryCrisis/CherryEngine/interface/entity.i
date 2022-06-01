%{
	#include "entity.hpp"
	#include "object.hpp"
%}

%include std_string.i
%include std_vector.i

%include cell.i
%include transform_comp.i
%include camera_comp.i
%include scripted_behaviour.i
%include model_renderer.i
%include light_component.i
%include object.i

%define COMP_STD_WRAP(COMP_T)
namespace std
{
	%template(COMP_T ## Vector) vector<COMP_T*>;
};
%enddef

%define COMP_TEMPLATE_WRAP(COMP_T)
COMP_PREFIX_WRAP(Get, COMP_T, GetBehaviour)
COMP_PREFIX_WRAP(Add, COMP_T, AddBehaviour)
COMP_PREFIX_WRAP(GetAllOf, COMP_T, GetBehavioursOfType)
%enddef

%define COMP_PREFIX_WRAP(prefix, COMP_T, funcSignature)
%csmethodmodifiers funcSignature<COMP_T> "private";
%template(prefix ## COMP_T) funcSignature<COMP_T>;
%enddef

COMP_STD_WRAP(CameraComponent)
COMP_STD_WRAP(Transform)
COMP_STD_WRAP(ScriptedBehaviour)
COMP_STD_WRAP(ModelRenderer)
COMP_STD_WRAP(LightComponent)

%nodefaultctor Entity;
class Entity : public Object
{
	%csmethodmodifiers GetName() "private";

private:
	std::string m_name;
	std::vector<Behaviour*> m_behaviours;

public:
	Cell* m_cell = nullptr;

	virtual ~Entity();

	template <class CompT>
	CompT* AddBehaviour();

	template <class CompT>
	CompT* GetBehaviour();

	template <class CompT>
	std::vector<CompT*> GetBehavioursOfType();

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
	COMP_TEMPLATE_WRAP(ModelRenderer)
	COMP_TEMPLATE_WRAP(LightComponent)

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

	public Behaviour AddBehaviour(System.Type type)
	{
		if (type == typeof(Transform))
			return AddTransform();

		if (type == typeof(CameraComponent))
			return AddCameraComponent();

		if (type == typeof(ModelRenderer))
			return AddModelRenderer();

		if (type == typeof(LightComponent))
			return AddLightComponent();

		return AddScript(type.Name);
	}

	public T AddBehaviour<T>() where T : Behaviour => AddBehaviour(typeof(T)) as T;

	public Behaviour GetBehaviour(System.Type type)
	{
		if (type == typeof(Transform))
			return GetTransform();

		if (type == typeof(CameraComponent))
			return GetCameraComponent();

		if (type == typeof(ModelRenderer))
			return GetModelRenderer();
			
		if (type == typeof(LightComponent))
			return GetLightComponent();

		return GetScript(type.Name);
	}

	public T GetBehaviour<T>() where T : Behaviour => GetBehaviour(typeof(T)) as T;
	%}
};
