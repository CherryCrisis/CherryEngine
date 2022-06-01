#pragma once

#include <array>
#include <string>

#include "core/panel.hpp"
#include "input_manager.hpp"
#include "serializer.hpp"

class ProjectSettingsDisplayer : public Panel
{
private:
	class General : public PanelCategory 
	{
	public:
		char name[32] = "MyCherryGame";
		char version[32] = "0.0.1";
		char company[32] = "Cherry";

		General(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;
	};

	class Input : public PanelCategory
	{
	private:
		std::vector<int> m_axisPosIndex;
		std::vector<int> m_axisNegIndex;
		std::vector<int> m_inputsIndex;

	public:
		InputManager::InputContext* userContext = nullptr;

		Input(std::string name = "default");
		~Input();
		void Fill() override;
		void CreateAction(int& type);
		void CreateButtons(const char* name);
		void CreateAxes(const char* name);
		void SetButtons();
		void SetAxes();
	};

	class Audio : public PanelCategory
	{
	public:
		bool	m_isUsingSpatialization = true;
		float	m_globalVolume = 100.f;

		Audio(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;
	};

	class Physics : public PanelCategory
	{
	public:
		float m_gravity = 9.81f;

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

	class ResourceViewer : public PanelCategory
	{
	public:
		ResourceViewer(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;
	};

	int m_selectedCategory = 0;

	std::array<PanelCategory*, 7> m_categories;

public:
	ProjectSettingsDisplayer(bool spawnOpened);
	~ProjectSettingsDisplayer();

	ProjectSettingsPack GetBuildSettings();
	void  ApplyBuildSettings(ProjectSettingsPack pack); 

	void Render() override;
};


