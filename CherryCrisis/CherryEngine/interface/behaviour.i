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

	%proxycode
	%{
		public Entity host { get => GetHost(); }

		public override string ToString() => System.String.Format("{0} ({1})", base.ToString(), host);
	%}
};

%csmethodmodifiers GetHost() "private";
