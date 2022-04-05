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
	
	void AddLog(ELogType type, const char* message);

	%proxycode %{

	public void Log(ELogType type, params object[] messages) => AddLog(type, string.Join(" ; ", messages));
	public void Info(params object[] messages) => AddLog(ELogType.INFO, string.Join(" ; ", messages));
	public void Warning(params object[] messages) => AddLog(ELogType.WARNING, string.Join(" ; ", messages));
	public void Error(params object[] messages) => AddLog(ELogType.ERROR, string.Join(" ; ", messages));
	%}
};