#pragma once

#include <glad/gl.h>

#include "renderpass_interface.hpp"

#include "mesh.hpp"

class ElementMeshGenerator
{
protected:
	struct GPUMeshBasic : GPUMesh
	{
		GLuint VAO = 0u, VBO = 0u, EBO = 0u;
	};

public:
	int Generate(Mesh* toGenerate);
};