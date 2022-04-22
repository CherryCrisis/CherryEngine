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
private:
	std::string m_name = "Entity";
	std::unordered_multimap<std::type_index, Behaviour*> m_behaviours;
public:
	Cell* m_cell = nullptr;

	Entity() = default;
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

	void Initialize();
	void Update();
	void Destroy();

	void SetName(std::string name) { m_name = name; }
	std::string GetName() { return m_name; }
};

#include "entity.inl"