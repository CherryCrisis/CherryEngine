#pragma once

#include <glad/gl.h>

#include "renderpass_interface.hpp"

#include "mesh.hpp"

class ElementMeshGenerator
{
public:
	struct GPUMeshBasic : public GPUMesh
	{
		GLuint VAO = 0u, VBO = 0u, EBO = 0u;
		GLsizei indicesCount = 0u;

		~GPUMeshBasic();
	};

	virtual bool Generate(Mesh* toGenerate);
	virtual bool Destroy(Mesh* toGenerate);
};