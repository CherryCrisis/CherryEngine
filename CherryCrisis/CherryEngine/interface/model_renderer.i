%{
	#include "mesh_renderer.hpp"
%}

%include mesh_renderer.i

%nodefaultctor ModelRenderer;

class ModelRenderer : public MeshRenderer
{
public:
	void SetMeshFromPath(std::string meshPath);
	std::string GetMeshPath();

	void LoadModelFromPath(std::string modelPath);
	std::string GetModelPath();
};
