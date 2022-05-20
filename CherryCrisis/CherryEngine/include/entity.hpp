#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <cherry_macros.hpp>

#include "uuid.hpp"
#include "event.hpp"
#include "object.hpp"
#include "behaviour.hpp"

class Scene;
class Behaviour;

template<class CompT>
concept BehaviourT = std::is_base_of_v<Behaviour, CompT>;

class Cell;

class CCENGINE_API Entity : public Object
{
protected:
	std::string m_name = m_defaultName;
	std::unordered_multimap<std::string, Behaviour*> m_behaviours;

public:
	static std::string m_defaultName;
	Cell* m_cell = nullptr;

	Entity(const std::string& name, Cell* cell, CCUUID uuid = {});
	virtual ~Entity();

	template <BehaviourT CompT>
	CompT* AddBehaviour();

	template <BehaviourT CompT>
	CompT* AddBehaviour(const std::string& componentTypeName);

	template <BehaviourT CompT>
	CompT* GetBehaviour();
	Behaviour* GetBehaviour(const std::string& componentTypeName);

	bool RemoveBehaviour(Behaviour* behaviour);

	std::vector<Behaviour*> GetAllBehaviours();

	std::vector<Behaviour*> GetBehavioursOfType(const std::string& componentTypeName);

	template <BehaviourT CompT>
	std::vector<CompT*> GetBehavioursOfType();

	template <BehaviourT CompT>
	void SubscribeComponent(CompT* behaviour);
	void SubscribeComponent(Behaviour* behaviour, const std::string& componentTypeName);

	template <BehaviourT CompT>
	bool HasBehaviour();

	template <BehaviourT CompT>
	bool TryGetBehaviour(CompT*& componentToReturn);

	template <BehaviourT CompT>
	CompT* GetOrAddBehaviour();

	Event<> m_OnAwake;
	Event<> m_OnStart;
	Event<> m_OnTick;
	Event<> m_OnDestroyed;
	Event<Cell*> m_OnCellAdded;
	Event<Cell*> m_OnCellRemoved;

	Event<> m_OnSelected;
	Event<> m_OnUnselected;

	Event<Entity*> m_OnCollisionEnter;
	Event<Entity*> m_OnCollisionStay;
	Event<Entity*> m_OnCollisionExit;
	Event<Entity*> m_OnTriggerEnter;
	Event<Entity*> m_OnTriggerExit;

	void Initialize();

	void Update();

	void OnCollisionEnter(Entity* other);
	void OnCollisionStay(Entity* other);
	void OnCollisionExit(Entity* other);
	void OnTriggerEnter(Entity* other);
	void OnTriggerExit(Entity* other);

	void Destroy();

	void SetName(std::string name) { m_name = name; }
	std::string GetName() const { return m_name; }
};

#include "entity.inl"
