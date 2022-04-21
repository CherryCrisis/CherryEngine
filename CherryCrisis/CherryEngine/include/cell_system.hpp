#pragma once

#include "cherry_macros.hpp"

#include <unordered_map>

#include "singleton.hpp"
#include "cell.hpp"

class CCENGINE_API CellSystem : public Singleton<CellSystem>
{
private:
	Debug* m_debug = nullptr;

	std::string m_defaultCell = "Default";

public:
	std::unordered_map<std::string, Cell> m_cells;

	CellSystem();

	Cell*	AddOrGetCell(const std::string& name);
	Cell*	GetCell(const std::string& name);
	bool	RemoveCell(const std::string& name);

	void	AddEntityToDefault(Entity* entity);
	void	MoveEntityFromCellToCell(const std::string& fromCell, const std::string& toCell, Entity* entity);
	void	MoveEntityFromCellToCell(Cell* fromCell, const std::string& toCell, Entity* entity);
	void	MoveEntityFromCellToCell(const std::string& fromCell, Cell* toCell, Entity* entity);
	void	MoveEntityFromCellToCell(Cell* fromCell, Cell* toCell, Entity* entity);
};