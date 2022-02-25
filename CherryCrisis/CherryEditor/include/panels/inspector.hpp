#pragma once

#include <core/panel.hpp>

class Engine;


class Inspector : public Panel 
{
public:

	void Render() override;
	
	Engine* m_engine = nullptr;
};