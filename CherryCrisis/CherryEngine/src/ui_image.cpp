#include <pch.hpp>

#include "ui_image.hpp"

#include "resource_manager.hpp"

#include "element_mesh_generator.hpp"
#include "rendering_pipeline_interface.hpp"
#include "ui_renderpass.hpp"

UIImage::UIImage(const char* filepath)
{
	m_texture = ResourceManager::GetInstance()->AddResource<Texture>(filepath, true);

    m_mesh = ResourceManager::GetInstance()->AddResource<Mesh>("CC_NormalizedQuad",true, EMeshShape::QUAD,.5f,.5f);
    
    //Generate GL buffers
    ElementMeshGenerator generator;
    generator.Generate(m_mesh.get());

    auto gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(m_mesh->m_gpuMesh.get());

    SetName("Image");
}

void UIImage::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
    if (!m_mesh || !m_texture)
		return;

	pipeline->SubscribeToPipeline<UIRenderPass>(this);
}

void UIImage::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
    pipeline->UnsubscribeToPipeline<UIRenderPass>(this);
}