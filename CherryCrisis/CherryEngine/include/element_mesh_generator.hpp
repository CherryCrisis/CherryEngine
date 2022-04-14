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
	virtual bool Generate(Mesh* toGenerate);
	virtual bool Destroy(Mesh* toGenerate);
};