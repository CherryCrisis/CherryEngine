#pragma once

#include "cherry_macros.hpp"

#include <unordered_map>

#include "singleton.hpp"
#include "cell.hpp"

class CCENGINE_API CellSystem : public Singleton<CellSystem>
{
private:
	std::unordered_map<std::string, Cell> m_cells;

public:
	CellSystem() {}

	void AddCell(const std::string& name);
	void RemoveCell(const std::string& name);
	void LinkCells(Cell& lhs, Cell& rhs);
};