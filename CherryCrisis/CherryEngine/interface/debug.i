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
	static void AddLog(ELogType logType, const char* message, unsigned int line, const char* file, const char* function);

	%proxycode %{
	public static void Log(ELogType type, object message,
        [System.Runtime.CompilerServices.CallerLineNumber] uint sourceLineNumber = 0u,
        [System.Runtime.CompilerServices.CallerFilePath] string sourceFilePath = "",
        [System.Runtime.CompilerServices.CallerMemberName] string memberName = "")
            => AddLog(type, message.ToString(), sourceLineNumber, sourceFilePath, memberName);

	public static void Info(object message,
        [System.Runtime.CompilerServices.CallerLineNumber] uint sourceLineNumber = 0u,
        [System.Runtime.CompilerServices.CallerFilePath] string sourceFilePath = "",
        [System.Runtime.CompilerServices.CallerMemberName] string memberName = "")
            => AddLog(ELogType.INFO, message.ToString(), sourceLineNumber, sourceFilePath, memberName);

	public static void Warning(object message,
        [System.Runtime.CompilerServices.CallerLineNumber] uint sourceLineNumber = 0u,
        [System.Runtime.CompilerServices.CallerFilePath] string sourceFilePath = "",
        [System.Runtime.CompilerServices.CallerMemberName] string memberName = "")
            => AddLog(ELogType.WARNING, message.ToString(), sourceLineNumber, sourceFilePath, memberName);

	public static void Error(object message,
        [System.Runtime.CompilerServices.CallerLineNumber] uint sourceLineNumber = 0u,
        [System.Runtime.CompilerServices.CallerFilePath] string sourceFilePath = "",
        [System.Runtime.CompilerServices.CallerMemberName] string memberName = "")
            => AddLog(ELogType.ERROR, message.ToString(), sourceLineNumber, sourceFilePath, memberName);
	%}
};