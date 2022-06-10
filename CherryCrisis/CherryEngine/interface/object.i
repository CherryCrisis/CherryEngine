%{
	#include "object.hpp"
%}

%feature("director") Object;

%nodefaultctor Object;

class Object
{
public:
	bool IsActive();
	void SetActive(bool value);
};