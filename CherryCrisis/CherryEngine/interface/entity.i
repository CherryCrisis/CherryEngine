%{
	#include "entity.hpp"
%}

%include std_string.i

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

	%proxycode
	%{
		public string name => GetName();

		public override string ToString() => name;
	%}


	%template(GetTransform) GetBehaviour<Transform>;
};

