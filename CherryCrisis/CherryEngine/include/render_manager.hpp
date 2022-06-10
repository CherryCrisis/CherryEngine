#pragma once

#include <set>
#include <memory>

#include <cherry_macros.hpp>

#include "singleton.hpp"

#include "shader_program.hpp"

struct	Framebuffer;
class	Viewer;
struct  GLFWwindow;
struct  GPUMesh;

struct MainWindow
{
	GLFWwindow* window;
	std::shared_ptr<ShaderProgram> shader;
	GPUMesh* quad;
};

class CCENGINE_API RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;

public:
	RenderManager();

	static void GenerateMainWindow(MainWindow& mainWindow);
	static void DrawMainWindow(const MainWindow& mainWindow);

	~RenderManager();
};
