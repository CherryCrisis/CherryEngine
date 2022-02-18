#pragma once

#include "Component.h"

#include "modelBase.hpp"

struct GPUMesh;

class CHERRY_API ModelRenderer : public Behaviour, public ARenderer
{
public:
	std::shared_ptr<Model> m_model;

	ModelRenderer();
	virtual ~ModelRenderer();

	void SetModel(std::shared_ptr<Model> newModel);
	void RemoveModel();

	void Draw() override;

	GPUMesh* m_gpuMesh = nullptr;
};