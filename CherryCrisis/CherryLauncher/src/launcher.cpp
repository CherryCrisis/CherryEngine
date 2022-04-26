
#include "launcher.hpp"

#include "portable-file-dialogs.hpp"
#undef far
#undef near
#undef ERROR
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h> 
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "command.hpp"
#include "utils.hpp"

void Launcher::AddProjectPath()
{
	Project project{};
	
	auto selection = pfd::open_file("Select a project").result();
	if (selection.empty())
		return;

	const char* cSelection = selection[0].c_str();
	std::filesystem::path path = cSelection;

	if (path.extension() != ".cherryproject") 
	{
		AddProjectPath();
		return;
	}

	project.path = path;
	project.name = path.filename().string();

	m_projects.push_back(project);
}

void Launcher::RemoveProjectPath()
{

}

void Launcher::StartProjectCreation()
{
	state = ELauncherState::CREATION;
}

void Launcher::CreateProject(const std::string& path, const std::string& name) 
{
	std::string newPath = path + '/' + name;
	std::string projectfilePath = newPath + '/' + name+".cherryproject";
	Project project{ projectfilePath, name};
	m_projects.push_back(project);

	//Create project Folder
	CreateFolder(path, name);
	
	CreateFolder(newPath, "Assets");
	CreateFolder(newPath, "Cache");
	
	//Create cherryproject 
	std::ofstream out(projectfilePath);

	YAML::Node save;

	save["Version"] = version;

	out << save;
	out.close();

	glfwSetWindowShouldClose(m_window, true);
	project.Open();
}

void Launcher::OpenProject()
{

}

void Launcher::DeleteProject() 
{

}

void Launcher::ReadLauncherInfos() 
{
	if (!std::filesystem::exists("launcher.meta"))
		return;

	YAML::Node loader = YAML::LoadFile("launcher.meta");

	for (YAML::const_iterator it = loader["projects"].begin(); it != loader["projects"].end(); ++it) 
	{
		Project project{};

		project.path = it->as<std::string>();
		project.name = project.path.filename().string();

		m_projects.push_back(project);
	}
}

void Launcher::WriteLauncherInfos() 
{
	YAML::Node save;
	for (const Project& project : m_projects) 
	{
		save["projects"].push_back(project.path.string());
	}
	std::ofstream out("launcher.meta");

	out << save;
	out.close();
}

std::vector<Project> Launcher::GetProjectList() const
{
	return m_projects;
}

void Project::Open() const
{
	std::filesystem::path folderPath = std::filesystem::path(path).parent_path();
	call("open", "CherryEditor.exe", folderPath.string().c_str());
}