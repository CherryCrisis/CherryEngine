#pragma once

#include "Component.h"
#include "debug.hpp"
#include "engine.h"
#include "model_renderer.hpp"
#include "maths.hpp"

#ifdef CHERRYGP_EXPORTS
#define CHERRY_API __declspec(dllexport)
#else
#define CHERRY_API __declspec(dllimport)
#endif
