
#pragma once

#include <array>
#include <string>

#include "core/panel.hpp"
#include "input_manager.hpp"

class ProjectSettingsDisplayer : public Panel
{
private:
	std::array<PanelCategory*, 7> m_categories;

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
	private:
		std::vector<int> m_axisPosIndex;
		std::vector<int> m_axisNegIndex;
		std::vector<int> m_inputsIndex;

	public:
		Input(std::string name = "default");
		void Fill() override;
		void CreateAction(InputManager* IM, int& type);
		void CreateButtons(InputManager* IM, const char* name);
		void CreateAxes(InputManager* IM, const char* name);
		void SetButtons(InputManager* IM);
		void SetAxes(InputManager* IM);

		InputManager::KeyboardContext* userContext = nullptr;
	};

	class Audio : public PanelCategory
	{
	public:
		Audio(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;

		bool m_isUsingSpatialization = true;
		float m_globalVolume = 100.f;
	};

	class Physics : public PanelCategory
	{
	public:
		Physics(std::string name = "default") : PanelCategory(name) {}
		void Fill() override;

		float m_gravity = 9.81f;
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

public:
	ProjectSettingsDisplayer(bool spawnOpened);
	~ProjectSettingsDisplayer();
	void Render() override;

	// All the things + will have to serialize of all this shit
};

