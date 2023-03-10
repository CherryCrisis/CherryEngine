%{
	#include "scripted_behaviour.hpp"
	#include "behaviour.hpp"
%}

%include std_string.i

%include behaviour.i

class ScriptedBehaviour : public Behaviour
{
public:
	std::string GetScriptPath();

	%ignore ScriptedBehaviour();

	void SetScriptClass(const std::string& scriptName);

	%proxycode%{
	[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
	private extern static Behaviour GetStaticInstance(System.IntPtr cPtr);

	public Behaviour GetInstance()
	{
		return GetStaticInstance(swigCPtr.Handle);
	}
	%}
};