#include "pch.hpp"

#include "cell_system.hpp"

void CellSystem::AddCell(const std::string& name)
{
	m_cells[name] = Cell();
}

void CellSystem::RemoveCell(const std::string& name)
{
	if (m_cells.contains(name))
		m_cells.erase(name);
}