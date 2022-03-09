%{
	#include "entity.hpp"
%}

%include std_string.i

%define COMP_TEMPLATE_WRAP(COMPT)
COMP_PREFIX_WRAP(Get, COMPT, GetBehaviour)
COMP_PREFIX_WRAP(Add, COMPT, AddBehaviour)
%enddef

%define COMP_PREFIX_WRAP(prefix, COMPT, funcSignature)
%csmethodmodifiers funcSignature<COMPT> "private";
%template(prefix ## COMPT) funcSignature<COMPT>;
%enddef

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
	bool HasBehaviour();

	template <class CompT>
	bool TryGetBehaviour(CompT*& componentToReturn);

	template <class CompT>
	CompT* GetOrAddBehaviour();

	void Destroy();

	std::string GetName();

	COMP_TEMPLATE_WRAP(CameraComponent)
	COMP_TEMPLATE_WRAP(Transform)

	%proxycode %{
	public string name => GetName();

	public override string ToString() => name;

	public Component AddComponent(System.Type type)
	{
		if (type == typeof(Transform))
			return AddTransform();

		if (type == typeof(CameraComponent))
			return AddCameraComponent();

		return null;
	}

	public T AddComponent<T>() where T : Component => AddComponent(typeof(T)) as T;

	public Component GetComponent(System.Type type)
	{
		if (type == typeof(Transform))
			return GetTransform();

		if (type == typeof(CameraComponent))
			return GetCameraComponent();

		return null;
	}

	public T GetComponent<T>() where T : Component => GetComponent(typeof(T)) as T;
	%}
};
