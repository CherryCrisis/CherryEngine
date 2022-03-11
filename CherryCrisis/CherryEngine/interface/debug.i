%{
	#include "debug.hpp"

%}


class Debug
{
private:
	Debug(Debug& other) = delete;
	void operator=(const Debug&) = delete;
	Debug();

public:
	static Debug* GetInstance();
	
	void Log(const char* message);

	%proxycode %{
	public void Log(object message) => Log(message.ToString());

	public void Log(string separator, params object[] messages) => Log(string.Join(separator, messages));
	public void Log(params object[] messages) => Log(string.Join(" ; ", messages));
	%}
};