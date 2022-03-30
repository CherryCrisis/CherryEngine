%{
	#include "behaviour.hpp"
	#include "component.hpp"
%}

%include component.i

%feature("director") Behaviour;

class Behaviour : public Component
{
private:
	Entity& m_owner;

public:
	virtual ~Behaviour() = default;

	Entity& GetHost();

	%proxycode %{
	public Entity host { get => GetHost(); }

	public override string ToString() => System.String.Format("{0} ({1})", base.ToString(), host);

	public Component GetComponent(System.Type type) => host.GetComponent(type);
	public T GetComponent<T>() where T : Component => host.GetComponent<T>();

	public Component AddComponent(System.Type type) => host.AddComponent(type);
	public T AddComponent<T>() where T : Component => host.AddComponent<T>();
	%}
};

%csmethodmodifiers GetHost() "private";
