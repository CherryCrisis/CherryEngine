%{
	#include "behaviour.hpp"
	#include "object.hpp"
%}

%include object.i
%include entity.i

namespace std
{
	%template(EntityVector) vector<Entity*>;
};

%nodefaultctor Cell;

class Cell : public Object
{
private:
	std::vector<Entity*>	m_entities;

public:
	void Initialize();

	const std::vector<Entity*>& GetEntities();

	void SetName(const std::string& name);
	const std::string& GetName();

	%proxycode %{
		public override string ToString() => GetName();
	%}

};