#pragma once

#include <filesystem>
#include <vector>

struct GLFWwindow;

enum class ELauncherState
{
	MAIN,
	CREATION
};

class Project
{
public:

	std::filesystem::path path;
	std::string name;

	void Open() const ;
};

class Launcher
{
	std::vector<Project> m_projects;
	GLFWwindow* m_window = nullptr;
	std::string version = "1.0.0";

public:
	Launcher(GLFWwindow* window) : m_window(window) { ReadLauncherInfos(); }

	void AddProjectPath();
	void RemoveProjectPath();

	void StartProjectCreation(); 
	
	void CreateProject(const std::string& path, const std::string& name);
	void OpenProject();
	void DeleteProject();

	void WriteLauncherInfos();
	void ReadLauncherInfos();

	std::vector<Project> GetProjectList() const;

	ELauncherState state = ELauncherState::MAIN;
};