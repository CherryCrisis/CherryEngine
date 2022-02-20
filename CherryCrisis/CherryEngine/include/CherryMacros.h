#pragma once

#ifdef CHERRYGP_EXPORTS
#define CCSCRIPT_API __declspec(dllexport)
#else
#define CCSCRIPT_API __declspec(dllimport)
#endif

#ifdef CHERRYDLL_EXPORTS
#define CCENGINE_API __declspec(dllexport)
#else
#define CCENGINE_API __declspec(dllimport)
#endif
