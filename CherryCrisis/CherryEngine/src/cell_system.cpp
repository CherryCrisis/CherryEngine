#include "pch.hpp"

#include "cell_system.hpp"

#include "debug.hpp"
#include "entity.hpp"

CellSystem* Singleton<CellSystem>::currentInstance = nullptr;

CellSystem::CellSystem()
{
	m_debug = Debug::GetInstance();

	AddOrGetCell(m_defaultCell);
}

Cell* CellSystem::AddOrGetCell(const std::string& name)
{
	// TODO: cache variables
	if (!m_cells.contains(name))
		m_cells[name] = Cell();

	m_cells[name].SetName(name);
	m_cells[name].Initialize();

	return &m_cells[name];
}

Cell* CellSystem::GetCell(const std::string& name)
{
	if (!m_cells.contains(name))
		return nullptr;

	return &m_cells[name];
}

bool CellSystem::RenameCell(const std::string& oldName, const std::string& newName)
{
	if (oldName == m_defaultCell)
	{
		m_debug->AddLog(ELogType::WARNING, "Cannot rename default cell");
		return false;
	}

	if (m_cells.contains(oldName) && !m_cells.contains(newName))
	{
		auto cell = m_cells.extract(oldName);
		cell.key() = newName;
		m_cells.insert(move(cell));
		m_cells[newName].SetName(newName);

		return true;
	}
	return false;
}

bool CellSystem::RemoveCell(const std::string& name)
{
	if (name == m_defaultCell)
	{
		m_debug->AddLog(ELogType::WARNING, "Cannot remove default cell");
		return false;
	}

	if (m_cells.contains(name))
	{
		for (auto& entity : m_cells[name].GetEntities())
			MoveEntityFromCellToCell(name, m_defaultCell, entity);

		m_cells.erase(name);
		return true;
	}
	return false;
}

void CellSystem::AddEntityToDefault(Entity* entity)
{
	m_cells[m_defaultCell].AddEntity(entity);
}

void CellSystem::MoveEntityFromCellToCell(const std::string& fromCell, const std::string& toCell, Entity* entity)
{
	if (!m_cells.contains(fromCell) || !m_cells.contains(toCell))
		return;

	int i = m_cells[fromCell].PossessEntity(entity);
	if (i == -1)
	{
		m_debug->AddLog(ELogType::ERROR, "Trying to remove an entity from a cell not owning it");
		return;
	}

	i = m_cells[toCell].PossessEntity(entity);
	if (i != -1)
	{
		m_debug->AddLog(ELogType::ERROR, "Trying to move an entity to a cell already owning it");
		return;
	}

	m_cells[fromCell].RemoveEntity(entity);
	m_cells[toCell].AddEntity(entity);
}

void CellSystem::MoveEntityFromCellToCell(Cell* fromCell, const std::string& toCell, Entity* entity)
{
	if (!fromCell || !m_cells.contains(toCell))
		return;

	int i = fromCell->PossessEntity(entity);
	if (i == -1)
	{
		m_debug->AddLog(ELogType::ERROR, "Trying to remove an entity from a cell not owning it");
		return;
	}

	i = m_cells[toCell].PossessEntity(entity);
	if (i != -1)
	{
		m_debug->AddLog(ELogType::ERROR, "Trying to move an entity to a cell already owning it");
		return;
	}

	fromCell->RemoveEntity(entity);
	m_cells[toCell].AddEntity(entity);
}

void CellSystem::MoveEntityFromCellToCell(const std::string& fromCell, Cell* toCell, Entity* entity)
{
	if (!toCell || !m_cells.contains(fromCell))
		return;

	int i = m_cells[fromCell].PossessEntity(entity);
	if (i == -1)
	{
		m_debug->AddLog(ELogType::ERROR, "Trying to remove an entity from a cell not owning it");
		return;
	}

	i = toCell->PossessEntity(entity);
	if (i != -1)
	{
		m_debug->AddLog(ELogType::ERROR, "Trying to move an entity to a cell already owning it");
		return;
	}

	m_cells[fromCell].RemoveEntity(entity);
	toCell->AddEntity(entity);
}

void CellSystem::MoveEntityFromCellToCell(Cell* fromCell, Cell* toCell, Entity* entity)
{
	if (!toCell || !fromCell)
		return;

	int i = fromCell->PossessEntity(entity);
	if (i == -1)
	{
		m_debug->AddLog(ELogType::ERROR, "Trying to remove an entity from a cell not owning it");
		return;
	}

	i = toCell->PossessEntity(entity);
	if (i != -1)
	{
		m_debug->AddLog(ELogType::ERROR, "Trying to move an entity to a cell already owning it");
		return;
	}

	fromCell->RemoveEntity(entity);
	toCell->AddEntity(entity);
}
