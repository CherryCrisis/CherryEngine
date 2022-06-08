#pragma once

#include "behaviour.hpp"

#include "maths.hpp"

#include "portal_component.hpp"

#include "property.hpp"

class MeshRenderer;
class Transform;

class CCENGINE_API PortalTeleporterComponent : public Behaviour
{
public :
	struct EntityNode
	{
		Transform* m_transform = nullptr;
		MeshRenderer* m_meshRenderer = nullptr;
		std::vector<EntityNode> m_meshRendererChildren;
	};
protected:
	void PopulateMetadatas() override;

	void GenerateEntityNodesFromTransform(EntityNode* entityNode, Transform* transform);

	//Return root transform
	void CloneEntities(EntityNode* cloneMeshRendererNode, const EntityNode* meshRenderderNode, Cell* destCell, Scene* scene);
	void SetIsVisibleEntityNode(EntityNode* entityNode, bool isVisible);
public:
	void ReloadEntitiesClone();

	CCMaths::Vector3 m_previousOffsetFromPortal = Vector3::Zero;

	Transform* m_transform = nullptr;
	std::unique_ptr<EntityNode> m_entityNode = nullptr;
	std::unique_ptr<EntityNode> m_cloneEntityNode = nullptr;

	PortalTeleporterComponent();
	PortalTeleporterComponent(CCUUID& id);
	~PortalTeleporterComponent();

	void Initialize();
	void BindToSignals() override;
	void Start();

	void OnRemovedClonedEntities();

	void SetSliceParams(EntityNode* meshRendererNode, bool isSlice, const CCMaths::Vector3& sliceCentre, const CCMaths::Vector3& sliceNormal);
	
	void EnterPortal(const PortalComponent* linkedPortal, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale);
	void UpdateEntityMatrix(Transform* transform, const Matrix4& newWorldMatrix);
	void ExitPortal();

	void Teleport(PortalComponent* destPortal, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale);

	//CCProperty::CopyProperty<PortalComponent, Behaviour*> m_LinkedPortalProp{ this, &PortalComponent::SetLinkedPortal, &PortalComponent::GetLinkedPortal };

};
