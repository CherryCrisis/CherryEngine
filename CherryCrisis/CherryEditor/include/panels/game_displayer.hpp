#pragma once

#include "core/frame_displayer.hpp"

class EditorManager;


class GameDisplayer : public FrameDisplayer
{
public:
	bool m_isHovered = false;
	
	EditorManager* m_manager = nullptr;

	GameDisplayer(EditorManager* manager) : m_manager(manager) {}

	void Render() override;

	void Focus() override;
	void Unfocus() override;
};