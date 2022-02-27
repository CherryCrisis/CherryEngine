
#pragma once

#include "core/panel.hpp"
#include <array>
#include <string>

class ProjectSettingsDisplayer : public Panel
{
private:
	std::array<PanelCategory*, 6> m_categories;

	int m_selectedCategory = 0;

//-------------Nested Classes

	class General : public PanelCategory 
	{
	public:
		General(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;
	};

	class Input : public PanelCategory
	{
	public:
		Input(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;
	};

	class Audio : public PanelCategory
	{
	public:
		Audio(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;
	};

	class Physics : public PanelCategory
	{
	public:
		Physics(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;
	};

	class TagLayer : public PanelCategory
	{
	public:
		TagLayer(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;
	};

	class RenderPass : public PanelCategory
	{
	public:
		RenderPass(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;
	};

public:
	ProjectSettingsDisplayer(bool spawnOpened);
	~ProjectSettingsDisplayer();
	void Render() override;

	// All the things + will have to serialize of all this shit
};

