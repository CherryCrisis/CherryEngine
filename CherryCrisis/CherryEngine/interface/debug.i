%include "enums.swg"

%{
	#include "debug.hpp"

%}

enum class ELogType
{
	INFO,
	WARNING,
	ERROR,
};

class Debug
{
private:

public:
	static Debug* GetInstance();
	
	void AddLog(const char* message, ELogType type);

	%proxycode %{
	public void Log(object message, ELogType type) => AddLog(message.ToString(), type);

	public void Log(ELogType type, string separator, params object[] messages) => AddLog(string.Join(separator, messages), type);
	public void Log(ELogType type, params object[] messages) => AddLog(string.Join(" ; ", messages), type);
	%}
};