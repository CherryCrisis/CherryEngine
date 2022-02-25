#pragma once

#ifdef CCSCRIPT_EXPORTS
#define CCSCRIPT_API __declspec(dllexport)
#else
#define CCSCRIPT_API __declspec(dllimport)
#endif

#ifdef CCENGINE_EXPORTS
#define CCENGINE_API __declspec(dllexport)
#else
#define CCENGINE_API __declspec(dllimport)
#endif
