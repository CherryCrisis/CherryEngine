#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 
#include <glad/gl.h>
#include <iostream>
#include <filesystem>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "stb_image.h"

#include "launcher.hpp"
#include "portable-file-dialogs.hpp"
#undef far
#undef near
#undef ERROR

#include "utils.hpp"
void window_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

unsigned int Loadpreview(const char* filepath) 
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, STBI_rgb);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
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
    io.Fonts->AddFontFromFileTTF("Plumpfull.ttf", 12.f);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    Project* FocusedProject = nullptr;

    ImGui::GetStyle().Colors[ImGuiCol_Button]        = { .2f,.2f,.2f,1.f };
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = { .35f,.35f,.35f,1.f };
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]  = { .25f,.25f,.25f,1.f };

    unsigned int previewID = 0;

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
        ImGui::Begin("Launcher", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
        
        if (launcher.state == ELauncherState::MAIN) 
        {
            ImVec2 screen = ImGui::GetContentRegionAvail();
            ImGui::BeginChild("CustomMainBar", { screen.x, screen.y * .05f });
            
            ImGui::SetCursorPosX(screenWidth * .75f);
            if (ImGui::Button("Create Project", { screenWidth * .1f, screenHeight * .05f }))
            {
                launcher.StartProjectCreation();
            }   ImGui::SameLine();

            if (ImGui::Button("Add existing Project", { screenWidth * .1f, screenHeight * .05f }))
            {
                launcher.AddProjectPath();
            }

            ImGui::EndChild();
            ImGui::Separator();
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, screenWidth * (2.f / 12.f));
            ImGui::PushStyleColor(ImGuiCol_ChildBg, { .08f,.08f ,.08f ,1.f });
            ImGui::BeginChild("projectList");
            {
                for (Project& project : launcher.m_projects)
                {
                    ImVec4 buttonColor  = ImGui::GetStyle().Colors[ImGuiCol_Button];
                    ImVec4 buttonColorH = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
                    ImVec4 buttonColorA = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];

                    if (&project == FocusedProject)
                        buttonColor = buttonColorH = buttonColorA = { .1f,.1f,.1f,1.f };

                        ImGui::PushStyleColor(ImGuiCol_Button,        buttonColor);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColorH);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  buttonColorA);

                        if (ImGui::Button(project.name.c_str(), { ImGui::GetContentRegionAvail().x, 75.f })) 
                        {
                            FocusedProject = &project;

                            previewID = Loadpreview((FocusedProject->path.parent_path().string() + "\\preview.png").c_str());
                        }

                    ImGui::PopStyleColor(3);
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
            ImGui::NextColumn();
            ImGui::SetColumnWidth(1, screenWidth * (10.f / 12.f));
            if (FocusedProject)
            {
                ImGui::Dummy({ 1.f,40.f });
                ImGui::BeginTable("project infos", 3, ImGuiTableFlags_SizingFixedFit);
                ImGui::TableNextColumn();
                ImGui::Dummy({ 20.f,1.f });
                ImGui::TableNextColumn();
                // Project Infos
                ImGui::SetWindowFontScale(2.f);
                ImGui::Text(String::ExtractKey(FocusedProject->name,'.').c_str());
                ImGui::SetWindowFontScale(1.f);
                ImGui::Text("Short Description");
                ImGui::Text(FocusedProject->path.string().c_str()); ImGui::SameLine();                ImGui::Dummy({ 35.f,1.f });
                ImGui::TableNextColumn();
                //Preview
                ImGui::Image((ImTextureID)previewID, { ImGui::GetColumnWidth(), ImGui::GetContentRegionAvail().y * .7f });
                ImGui::Dummy({1.f,ImGui::GetContentRegionAvail().y * .15f});
                //Down Buttons
                ImVec2 buttonSize = { ImGui::GetContentRegionAvail().x * .2f, ImGui::GetContentRegionAvail().x * .05f };
                if (ImGui::Button("Open", buttonSize))   FocusedProject->Open(); ImGui::SameLine();
                ImGui::Dummy({buttonSize.x /4.5f, 1.f}); ImGui::SameLine();
                if (ImGui::Button("Open Project Folder", buttonSize)) FocusedProject->OpenLocation(); ImGui::SameLine();
                ImGui::Dummy({ buttonSize.x / 4.5f, 1.f }); ImGui::SameLine();
                if (ImGui::Button("Remove From list", buttonSize)) launcher.RemoveProjectPath(*FocusedProject); ImGui::SameLine();
                ImGui::Dummy({ buttonSize.x / 4.5f, 1.f }); ImGui::SameLine();
                if (ImGui::Button("Delete", buttonSize)) launcher.DeleteProject(*FocusedProject); 
                
                ImGui::EndTable();
            }
            ImGui::Columns(1);
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

        //ImGui::ShowDemoWindow();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    launcher.WriteLauncherInfos();
    glDeleteTextures(1, &previewID);
    io.Fonts->ClearFonts();
    // Terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
