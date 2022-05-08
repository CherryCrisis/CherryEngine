#pragma once

#include <memory>
#include <string>
#include <fstream>
#include <unordered_map>

#include <cherry_macros.hpp>

#include "uuid.hpp"
#include "event.hpp"
#include "object.hpp"

class Behaviour;
class Cell;

class CCENGINE_API Entity : public Object
{
protected:
	std::string m_name = "Entity";
	std::unordered_multimap<std::type_index, Behaviour*> m_behaviours;

public:
	Cell* m_cell = nullptr;

	Entity();
	Entity(const std::string& name, CCUUID m_uuid = {});
	virtual ~Entity();

	template <class CompT>
	CompT* AddBehaviour();

	template <class CompT>
	CompT* GetBehaviour();

	bool RemoveBehaviour(Behaviour* behaviour);

	std::vector<Behaviour*> GetAllBehaviours();

	template <class CompT>
	std::vector<CompT*> GetBehaviours();

	template <class CompT>
	void SubscribeComponent(CompT* behaviour);

	template <class CompT>
	bool HasBehaviour();

	template <class CompT>
	bool TryGetBehaviour(CompT*& componentToReturn);

	template <class CompT>
	CompT* GetOrAddBehaviour();

	Event<> m_OnAwake;
	Event<> m_OnStart;
	Event<> m_OnTick;
	Event<> m_OnDestroyed;
	Event<Cell*> m_OnCellAdded;
	Event<Cell*> m_OnCellRemoved;

	Event<> m_OnCollisionEnter;
	Event<> m_OnCollisionStay;
	Event<> m_OnCollisionExit;
	Event<> m_OnTriggerEnter;
	Event<> m_OnTriggerExit;

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