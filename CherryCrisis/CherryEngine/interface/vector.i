%typemap(cscode) TVector %{
	public static $csclassname operator +($csclassname lhs, $csclassname rhs) => lhs.operator_add(rhs);
	public static $csclassname operator +($csclassname lhs, float rhs) => lhs.operator_add(rhs);

	public static $csclassname operator -($csclassname lhs, $csclassname rhs) => lhs.operator_substract(rhs);
	public static $csclassname operator -($csclassname lhs, float rhs) => lhs.operator_substract(rhs);

	public static $csclassname operator *($csclassname lhs, float rhs) => lhs.operator_multiply(rhs);
	public static $csclassname operator *(float lhs, $csclassname rhs) => rhs.operator_multiply(lhs);
%}

%define WRAP_VECTOR(CPPCLASS)

%ignore CPPCLASS::operator +=;
%ignore CPPCLASS::operator -=;
%rename(operator_add) CPPCLASS::operator +;
%rename(operator_substract) CPPCLASS::operator -;
%rename(operator_multiply) CPPCLASS::operator *;

%apply TVector { CPPCLASS };

%enddef

%include vector2.i
%include vector3.i
%include vector4.i