#pragma once

#include <vector>
#include <memory>

class Model;
struct ModelNode;

namespace CCModelLoader
{
	void LoadModel(const char* filepath, ModelNode* rootModels, std::vector<std::shared_ptr<Model>>& models);
}

