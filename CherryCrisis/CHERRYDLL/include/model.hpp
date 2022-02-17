#pragma once

#include <vector>
#include <map>

#include "resource.hpp"

class Mesh;
class Model;

struct aiScene;
struct aiNode;

class Model : public Resource
{
private:
	Model(const char* filepath) : Resource(filepath) {}

	std::shared_ptr<Mesh> m_mesh;

public:
	~Model();

	static Resource* Create(const char* filepath, const aiScene* assimpScene, const aiNode* assimpNode);
};
