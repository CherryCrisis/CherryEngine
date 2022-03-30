#include "pch.hpp"

#include "cell_system.hpp"

CellSystem* Singleton<CellSystem>::currentInstance = nullptr;

void CellSystem::AddCell(const std::string& name)
{
	m_cells[name] = Cell();
}

void CellSystem::RemoveCell(const std::string& name)
{
	if (m_cells.contains(name))
		m_cells.erase(name);
}

void CellSystem::LinkCells(Cell& lhs, Cell& rhs)
{
	lhs.LinkToCell(rhs);
	rhs.LinkToCell(lhs);
}