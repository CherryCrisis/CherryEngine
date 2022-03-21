#pragma once

#include <memory>
#include <string>
#include <vector>

#include <cherry_macros.hpp>

class Behaviour;

class CCENGINE_API Entity
{
private:
	std::string m_name = "Entity";
	std::vector<Behaviour*> m_behaviours;

public:
	Entity() = default;
	Entity(const std::string& name);
	virtual ~Entity();

	template <class CompT>
	CompT* AddBehaviour();

	template <class CompT>
	CompT* GetBehaviour();

	std::vector<Behaviour*>& GetBehaviours() { return m_behaviours; }
	const std::vector<Behaviour*>& GetBehaviours() const { return m_behaviours; }

	template <class CompT>
	bool HasBehaviour();

	template <class CompT>
	bool TryGetBehaviour(CompT*& componentToReturn);

	template <class CompT>
	CompT* GetOrAddBehaviour();

	void Start();
	void Update();
	void Destroy();

	std::string GetName() { return m_name; }
};

#include "entity.inl"