#pragma once

#include <CherryMacros.h>

#include "Component.h"

#include "renderer.hpp"

#include "modelBase.hpp"

struct GPUMesh;

class Transform;

class CCENGINE_API ModelRenderer : public Behaviour, public ARenderer
{
private:
public:
	Transform* m_transform = nullptr;
	std::shared_ptr<Model> m_model;

	ModelRenderer();
	~ModelRenderer();

	void SetModel(std::shared_ptr<Model> newModel);
	void RemoveModel();

	void Draw() override;

	GPUMesh* m_gpuMesh = nullptr;
};