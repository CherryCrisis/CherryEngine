#pragma once

#include <unordered_set>

#include <glad/gl.h>

#include "subpipeline_interface.hpp"

#include "texture.hpp"
#include "light.hpp"
#include "mesh.hpp"

class CameraComponent;
class ModelRenderer;
class Material;

class BasicSubPipeline : public ASubPipeline
{
	std::unordered_set<ModelRenderer*>	m_modelRenderers;
	std::unordered_set<Light*> m_lights;
	
	CameraComponent* m_camera = nullptr;

public:
	struct GPUMeshBasic : GPUMesh
	{
		GLuint VAO = 0u, VBO = 0u, EBO = 0u;
	};

	struct GPUTextureBasic : GPUTexture
	{
		GLuint ID = 0u;
	};

	BasicSubPipeline(const char* name);

	template <typename RendererT>
	int Generate(RendererT* toGenerate)
	{
		static_assert(false, "RendererT generation is not implemented in BasicSubPipeline");
	}

	template <typename RendererT>
	void Remove(RendererT* toGenerate)
	{
		static_assert(false, "RendererT deletion is not implemented in BasicSubPipeline");
	}

	template <>
	int Generate(Light* toGenerate);

	template <>
	int Generate(CameraComponent* toGenerate);

	template <>
	int Generate(ModelRenderer* toGenerate);

	template <>
	int Generate(Mesh* toGenerate);

	template <>
	int Generate(Material* toGenerate);

	template <>
	int Generate(Texture* toGenerate);

	template <>
	void Remove(CameraComponent* toGenerate);

	template <>
	void Remove(ModelRenderer* toGenerate);

	template <>
	void Remove(Light* toGenerate);

	void Execute() override;
};