%{
	#include "behaviour.hpp"
%}

class Behaviour
{
private:
	Entity& m_owner;

public:
	Behaviour(Entity& owner);
	virtual ~Behaviour() = default;

	Entity& GetHost();

	%proxycode
	%{
		public Entity host { get => GetHost(); }

		public override string ToString() => System.String.Format("{0} ({1})", base.ToString(), host);
	%}
};

%csmethodmodifiers GetHost() "private";