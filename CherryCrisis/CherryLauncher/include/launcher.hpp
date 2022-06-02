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
	void OpenLocation() const;
};

class Launcher
{
	GLFWwindow* m_window = nullptr;
	std::string version = "1.0.0";

public:
	Launcher(GLFWwindow* window) : m_window(window) { ReadLauncherInfos(); }

	Project* AddProjectPath();
	void RemoveProjectPath(const Project& project);

	void StartProjectCreation(); 
	
	void CreateProject(const std::string& path, const std::string& name);
	void DeleteProject(const Project& project);

	void WriteLauncherInfos();
	void ReadLauncherInfos();

	Project* FindProject(const std::filesystem::path& path);
	int FindProjectIndex(const std::filesystem::path& path);

	std::vector<Project> m_projects;

	ELauncherState state = ELauncherState::MAIN;
};