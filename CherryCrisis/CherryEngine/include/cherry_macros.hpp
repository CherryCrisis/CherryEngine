#pragma once

#pragma warning( disable : 4251 )

#ifdef SWIG
	#define CCENGINE_API
#else
	#ifdef CCENGINE_EXPORTS
		#define CCENGINE_API __declspec(dllexport)
	#else
		#define CCENGINE_API __declspec(dllimport)
	#endif
#endif

#ifdef CCSCRIPT_EXPORTS
#define CCSCRIPT_API __declspec(dllexport)
#else
#define CCSCRIPT_API __declspec(dllimport)
#endif