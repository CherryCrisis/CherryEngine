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
%include audio_listener.i
%include audio_emitter.i
%include portal_component.i
%include portal_teleporter_component.i
%include rigidbody.i
%include box_collider.i
%include sphere_collider.i
%include capsule_collider.i
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
COMP_STD_WRAP(AudioListener)
COMP_STD_WRAP(AudioEmitter)
COMP_STD_WRAP(PortalComponent)
COMP_STD_WRAP(PortalTeleporterComponent)
COMP_STD_WRAP(Rigidbody)
COMP_STD_WRAP(BoxCollider)
COMP_STD_WRAP(SphereCollider)
COMP_STD_WRAP(CapsuleCollider)

%nodefaultctor Entity;
class Entity : public Object
{
	%csmethodmodifiers GetName() "private";

private:
	std::string m_name;
	std::vector<Behaviour*> m_behaviours;

public:
	%immutable; 
	Cell* m_cell = nullptr;

	virtual ~Entity();

	template <class CompT>
	CompT* AddBehaviour();

	template <class CompT>
	CompT* GetBehaviour();

	Behaviour* GetBehaviour(const std::string& componentTypeName);

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

	void SetCell(Cell* cell);

	COMP_TEMPLATE_WRAP(CameraComponent)
	COMP_TEMPLATE_WRAP(ScriptedBehaviour)
	COMP_TEMPLATE_WRAP(Transform)
	COMP_TEMPLATE_WRAP(ModelRenderer)
	COMP_TEMPLATE_WRAP(LightComponent)
	COMP_TEMPLATE_WRAP(AudioListener)
	COMP_TEMPLATE_WRAP(AudioEmitter)
	COMP_TEMPLATE_WRAP(PortalComponent)
	COMP_TEMPLATE_WRAP(PortalTeleporterComponent)
	COMP_TEMPLATE_WRAP(Rigidbody)
	COMP_TEMPLATE_WRAP(BoxCollider)
	COMP_TEMPLATE_WRAP(SphereCollider)
	COMP_TEMPLATE_WRAP(CapsuleCollider)

	%proxycode %{
	private Behaviour AddScript(string scriptPath)
	{
		ScriptedBehaviour script = AddScriptedBehaviour();
		script.SetScriptClass(scriptPath);

		return script.GetInstance();
	}

	private Behaviour GetScript(string scriptPath)
	{
        Behaviour behaviour = GetBehaviour(scriptPath);
		if (behaviour is null) 
			return null;

        global::System.IntPtr cPtr = Behaviour.getCPtr(behaviour).Handle;
        ScriptedBehaviour scriptedBehaviour = (cPtr == global::System.IntPtr.Zero) ? null : new ScriptedBehaviour(cPtr, false);

        return scriptedBehaviour.GetInstance();
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
			
		if (type == typeof(AudioListener))
			return AddAudioListener();

		if (type == typeof(AudioEmitter))
			return AddAudioEmitter();

		if (type == typeof(PortalComponent))
			return AddPortalComponent();

		if (type == typeof(PortalTeleporterComponent))
			return AddPortalTeleporterComponent();

		if (type == typeof(Rigidbody))
			return AddRigidbody();

		if (type == typeof(BoxCollider))
			return AddBoxCollider();

		if (type == typeof(SphereCollider))
			return AddSphereCollider();

		if (type == typeof(CapsuleCollider))
			return AddCapsuleCollider();
			
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
						
		if (type == typeof(AudioListener))
			return GetAudioListener();
						
		if (type == typeof(AudioEmitter))
			return GetAudioEmitter();
									
		if (type == typeof(PortalComponent))
			return GetPortalComponent();

		if (type == typeof(PortalTeleporterComponent))
			return GetPortalTeleporterComponent();
			
		if (type == typeof(Rigidbody))
			return GetRigidbody();
			
		if (type == typeof(BoxCollider))
			return GetBoxCollider();
			
		if (type == typeof(SphereCollider))
			return GetSphereCollider();
			
		if (type == typeof(CapsuleCollider))
			return GetCapsuleCollider();
			
		return GetScript(type.Name);
	}

	public T GetBehaviour<T>() where T : Behaviour => GetBehaviour(typeof(T)) as T;
	%}
};
