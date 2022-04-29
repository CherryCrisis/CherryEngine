#pragma once

#include <glad/gl.h>

#include "renderpass_interface.hpp"

#include "element_mesh_generator.hpp"

class Mesh;

class CCENGINE_API ElementTBNGenerator : public ElementMeshGenerator
{
public:
	bool Generate(Mesh* toGenerate) override;
};