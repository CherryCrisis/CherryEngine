%{
	#include "behaviour.hpp"
	#include "object.hpp"
%}

%include object.i

%feature("director") Behaviour;

class Behaviour : public Object
{
private:
	Entity& m_owner;

public:
	virtual ~Behaviour() = default;

	Entity& GetHost();

	%proxycode %{
	public Entity host { get => GetHost(); }

	public override string ToString() => System.String.Format("{0} ({1})", base.ToString(), host);

	public Behaviour GetComponent(System.Type type) => host.GetComponent(type);
	public T GetComponent<T>() where T : Behaviour => host.GetComponent<T>();

	public Behaviour AddComponent(System.Type type) => host.AddComponent(type);
	public T AddComponent<T>() where T : Behaviour => host.AddComponent<T>();
	%}
};

%csmethodmodifiers GetHost() "private";
