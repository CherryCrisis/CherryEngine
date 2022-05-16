#include <pch.hpp>

#include "ui_image.hpp"

#include "resource_manager.hpp"

#include "element_mesh_generator.hpp"
#include "rendering_pipeline_interface.hpp"
#include "ui_renderpass.hpp"
#include "camera_component.hpp"

UIImage::UIImage(const char* filepath)
{
    Init(filepath);
}

UIImage::UIImage() 
{
    Init();
}

void UIImage::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
    if (!m_mesh || !m_texture)
		return;

    UIItem::SubscribeToPipeline(pipeline);
    pipeline->SubscribeToPipeline<UIRenderPass>(this);
}

void UIImage::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
    UIItem::UnsubscribeToPipeline(pipeline);
    pipeline->UnsubscribeToPipeline<UIRenderPass>(this);
}

void UIImage::Delete() 
{
    CameraComponent* cam = CameraComponent::GetMainCamera();
    if (!cam)
        return;

    UnsubscribeToPipeline(cam->m_camera.m_pipeline.get());
}

void UIImage::Init(const std::string& str) 
{
    std::string filepath = str.size() > 0 ? str : "Internal/Icons/shader_icon.png";
    PopulateMetadatas();
  
    SetTexture(filepath);
    m_mesh = ResourceManager::GetInstance()->AddResource<Mesh>("CC_NormalizedQuad", true, EMeshShape::QUAD, .5f, .5f);

    //Generate GL buffers
    ElementMeshGenerator generator;
    generator.Generate(m_mesh.get());

    auto gpuMesh = static_cast<ElementMeshGenerator::GPUMeshBasic*>(m_mesh->m_gpuMesh.get());

    SetName("Image");

    CameraComponent* cam = CameraComponent::GetMainCamera();
    if (!cam)
        return;

    SubscribeToPipeline(cam->m_camera.m_pipeline.get());
}

void UIImage::SetTexture(const std::string& filepath) 
{
    m_texture = ResourceManager::GetInstance()->AddResource<Texture>(filepath.c_str(), true);
    m_texturePath = m_texture->m_filepath.string();
}

void UIImage::PopulateMetadatas() 
{
    UIItem::PopulateMetadatas();

    m_metadatas.SetProperty("Texture", &TexturePath);
}