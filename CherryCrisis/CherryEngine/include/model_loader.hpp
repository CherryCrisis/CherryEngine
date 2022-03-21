#pragma once

#include <vector>
#include <memory>
#include <string>

#include "model.hpp"
#include "model_base.hpp"
//class Model;
//struct ModelNode;

namespace CCModelLoader
{
	void LoadModel(const char* filepath, ModelNode** rootModels, std::vector<std::shared_ptr<Model>>& models);
}

