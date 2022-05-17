#pragma once

#include "core/frame_displayer.hpp"

class EditorManager;

class GameDisplayer : public FrameDisplayer
{
public:
	GameDisplayer(EditorManager* manager) : m_manager(manager) {}
	EditorManager* m_manager = nullptr;

	bool m_isHovered = false;

	void Render() override;

	void Focus() override;
	void Unfocus() override;
};