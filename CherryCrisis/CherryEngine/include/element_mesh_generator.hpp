#pragma once

#include <glad/gl.h>

#include "renderpass_interface.hpp"

#include "mesh.hpp"

class ElementMeshGenerator
{
protected:
	struct GPUMeshBasic : public GPUMesh
	{
		GLuint VAO = 0u, VBO = 0u, EBO = 0u;

		~GPUMeshBasic();
	};

public:
	int Generate(Mesh* toGenerate);
	int Destroy(Mesh* toGenerate);
};