
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
	
	std::string str = String::ExtractValue(selection[0], '.');
	if (str != "cherryproject") 
	{
		AddProjectPath();
		return;
	}

	project.path = cSelection;
	project.name = project.path.filename().string();

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

}

void Launcher::WriteLauncherInfos() 
{

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