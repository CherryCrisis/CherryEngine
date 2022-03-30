#pragma once

#include "core/frame_displayer.hpp"

class EditorManager;
class SceneDisplayer : public FrameDisplayer
{
public:
	EditorManager* manager = nullptr;
	void Render() override;
};