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

	public Behaviour GetBehaviour(System.Type type) => host.GetBehaviour(type);
	public T GetBehaviour<T>() where T : Behaviour => host.GetBehaviour<T>();

	public Behaviour AddBehaviour(System.Type type) => host.AddBehaviour(type);
	public T AddBehaviour<T>() where T : Behaviour => host.AddBehaviour<T>();
	%}
};

%csmethodmodifiers GetHost() "private";
