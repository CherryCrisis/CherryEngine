
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 
#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "launcher.hpp"

#include "portable-file-dialogs.hpp"
#undef far
#undef near
#undef ERROR

void window_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    int screenWidth = 1200;
    int screenHeight = 1000;

    if (!glfwInit())
    {
        printf("glfwInit failed\n");
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Cherry Crisis Launcher", nullptr, nullptr);
    if (window == nullptr)
    {
        printf("glfwCreateWindow failed\n");
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (gladLoaderLoadGL() == 0)
    {
        printf("gladLoaderLoadGL failed\n");
        return -1;
    }
    if (GLAD_GL_KHR_debug)
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glfwSetWindowSizeCallback(window, window_size_callback);
    
    Launcher launcher{ window };

    //------- ImGui Setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    while (glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();

        glfwGetWindowSize(window, &screenWidth, &screenHeight);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        // do the launcher things
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("Launcher", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
        
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2.f);
        ImGui::Text("Cherry Crisis Launcher");
        


        if (launcher.state == ELauncherState::MAIN) 
        {
            if (ImGui::Button("Create Project"))
            {
                launcher.StartProjectCreation();
            }   ImGui::SameLine();

            if (ImGui::Button("Add existing Project"))
            {
                launcher.AddProjectPath();
            }

            ImGui::Separator();
            ImGui::Text("Projects:");
            ImGui::Separator();

            for (const Project& project : launcher.GetProjectList())
            {
                if (ImGui::BeginMenu(project.name.c_str()))
                {
                    if (ImGui::MenuItem("Open"))   project.Open();
                    if (ImGui::MenuItem("Open Project Folder")) project.OpenLocation();
                    if (ImGui::MenuItem("Remove From list")) launcher.RemoveProjectPath(project);
                    if (ImGui::MenuItem("Delete")) launcher.DeleteProject(project);
                    ImGui::EndMenu();
                }
            }
        }

        else if (launcher.state == ELauncherState::CREATION) 
        {
            ImGui::Text("Create Project:");

            static char name[32] = "";
            ImGui::InputText("Project Name", name, IM_ARRAYSIZE(name));

            static char path[32] = "";
            ImGui::InputText("Project Folder", path, IM_ARRAYSIZE(path));
            ImGui::SameLine(); if (ImGui::Button("..."))
            {
                auto selection = pfd::select_folder("Select a folder").result();
                const char* cSelection = selection.c_str();

                if (!selection.empty())
                    memcpy(path, cSelection, sizeof(char) * strlen(cSelection));
            }

            if (ImGui::Button("Create")) 
            {
                launcher.CreateProject(std::string(path), std::string(name));
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) 
            {
                launcher.state = ELauncherState::MAIN;
            }
        }
        

        ImGui::End();
        ImGui::PopStyleVar(1);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    launcher.WriteLauncherInfos();
    // Terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
