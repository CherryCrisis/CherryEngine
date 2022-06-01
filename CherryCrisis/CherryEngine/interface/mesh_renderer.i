%{
	#include "behaviour.hpp"
	#include "maths.hpp"
%}

%include <std_shared_ptr.i>

%include behaviour.i
%include mesh.i
%include material.i

%nodefaultctor MeshRenderer;

class MeshRenderer : public Behaviour
{
public:
	%immutable;
	std::shared_ptr<Material>	m_material = nullptr;
	
	%immutable;
	std::shared_ptr<Mesh>		m_mesh = nullptr;

	Transform* m_transform = nullptr;

	virtual void SetMesh(std::shared_ptr<Mesh> newMesh);
	virtual void RemoveMesh();

	virtual void SetMaterialFromPath(std::string meshPath);
	virtual std::string GetMaterialPath();

	virtual void SetMaterial(std::shared_ptr<Material> newMat);
	virtual void RemoveMaterial();
};