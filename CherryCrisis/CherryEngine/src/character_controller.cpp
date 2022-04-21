#include "pch.hpp"

#include "character_controller.hpp"

void CharacterController::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetProperty("m_radius",			&radius);
	m_metadatas.SetProperty("m_height",			&height);
	m_metadatas.SetProperty("m_slopeLimit",		&slopeLimit);
	m_metadatas.SetProperty("m_maxJumpHeight",	&maxJumpHeight);
	m_metadatas.SetProperty("m_contactOffset",	&contactOffset);
	m_metadatas.SetProperty("m_stepOffset",		&stepOffset);
	m_metadatas.SetProperty("m_density",		&density);
}