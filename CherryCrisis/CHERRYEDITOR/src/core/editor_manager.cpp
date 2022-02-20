
#include "core/editor_manager.hpp"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <string>

#include <imgui_impl_opengl3.h>

#include <glad/gl.h>
#include <tchar.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 

#include <CherryHeader.h>
#include "printer.hpp"
#include "scene.hpp"
#include "resourceManager.hpp"
#include "render_manager.hpp"

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    stbi_set_flip_vertically_on_load(true);
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
    
    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

EditorManager::EditorManager() 
{
    scene = ResourceManager::GetInstance()->AddResource<Scene>("scene de ouf", false);

    m_hierarchyDisplayer.SetScene(scene.get());
    
    int null = 0;

    if (!LoadTextureFromFile("../Internal/Icons/play_icon.png", &PlayIcon, &null, &null))
    {
        std::cout << "failed to load Play icon" << std::endl;
    }
    if (!LoadTextureFromFile("../Internal/Icons/pause_icon.png", &PauseIcon, &null, &null))
    {
        std::cout << "failed to load Pause icon" << std::endl;
    }
    if (!LoadTextureFromFile("../Internal/Icons/replay_icon.png", &ReplayIcon, &null, &null))
    {
        std::cout << "failed to load Replay icon" << std::endl;
    }
    if (!LoadTextureFromFile("../Internal/Icons/stop_icon.png", &StopIcon, &null, &null))
    {
        std::cout << "failed to load Stop icon" << std::endl;
    }
}

void EditorManager::LinkEngine(Engine* engine) 
{
    m_engine = engine;
    m_inspector.m_engine = engine;
}

void EditorManager::DisplayEditorUI(GLFWwindow* window) 
{
    HandleDocking();

    HandleMenuBar();
    m_browser.Render();
    m_logDisplayer.Render();
    m_inspector.Render();
    m_sceneDisplayer.Render();
    m_gameDisplayer.Render();
    m_hierarchyDisplayer.Render();

    HandleNotifications();

    if (isDemoOpened)   ImGui::ShowDemoWindow(&isDemoOpened);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorManager::HandleDocking() 
{
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Docking", NULL, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    ImGui::End();
}

void EditorManager::HandleMenuBar() 
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New")) {}
            if (ImGui::MenuItem("Open", "Ctrl+O")) {}
            if (ImGui::BeginMenu("Open Recent"))
            {
                ImGui::MenuItem("fish_hat.c");
                ImGui::MenuItem("fish_hat.inl");
                ImGui::MenuItem("fish_hat.h");
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Save As..")) {}

            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Project Settings")) { }
            if (ImGui::MenuItem("Preferences"))      { }
            ImGui::Separator();
            if (ImGui::MenuItem("Build")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows"))
        {
            if (ImGui::BeginMenu("Open"))
            {
                if (ImGui::MenuItem("Browser"))   { m_browser.Toggle(true); }
                if (ImGui::MenuItem("Hierarchy")) { isHierarchyOpened = true; }
                if (ImGui::MenuItem("Log"))       { m_logDisplayer.Toggle(true); }
                if (ImGui::MenuItem("Inspector")) { m_inspector.Toggle(true); }
                if (ImGui::MenuItem("Game"))      { m_gameDisplayer.Toggle(true); }
                if (ImGui::MenuItem("Scene"))     { m_sceneDisplayer.Toggle(true); }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }


        float size = ImGui::GetWindowHeight() - 4.f;

        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * .5f) - (size * .5f * 3.f));

        if (ImGui::ImageButton(m_engine->isPlaying ? (ImTextureID)StopIcon : (ImTextureID)PlayIcon, ImVec2(size, size), { 0,0 }, { 1,1 }, -1))
        {
            if (m_engine->isPlaying)
            {
                m_engine->Stop();
            }
            else
            {
                m_logDisplayer.TryClearOnPlay();
                m_engine->Launch();
            }

        } ImGui::SameLine();
        if (ImGui::ImageButton((ImTextureID)PauseIcon, ImVec2(size, size), { 0,0 }, { 1,1 }, -1))
        {

        } ImGui::SameLine();
        if (ImGui::ImageButton((ImTextureID)ReplayIcon, ImVec2(size, size), { 0,0 }, { 1,1 }, -1))
        {

        }

        ImGui::EndMainMenuBar();
    }
}

void EditorManager::HandleGraphWindow(GLFWwindow* window)
{
    if (!isHierarchyOpened)
        return;

    if (ImGui::Begin("Hierarchy", &isHierarchyOpened))
    {
        if (ImGui::Button("Close.."))
            ImGui::OpenPopup("Close?");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Close?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Files are unsaved!\n\n");
            ImGui::Separator();

            if (ImGui::Button("Close Anyway", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup(); glfwSetWindowShouldClose(window, true);
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        if (ImGui::Button("Add Printer"))
        {
            Printer* printer = new Printer();
            m_engine->behaviours.push_back(printer);
        }

        if (ImGui::Button("Success"))
            SendNotifiction("I am the Title ! %s", "and I am the content", ImGuiToastType_Success, 3.f);
        ImGui::SameLine();
        if (ImGui::Button("Warning"))
            SendNotifiction("I am the Title ! %s", "and I am the content", ImGuiToastType_Warning, 3.f);
        ImGui::SameLine();
        if (ImGui::Button("Error"))
            SendNotifiction("I am the Title ! %s", "and I am the content", ImGuiToastType_Error, 3.f);
        ImGui::SameLine();
        if (ImGui::Button("Info"))
            SendNotifiction("I am the Title ! %s", "and I am the content", ImGuiToastType_Info, 3.f);
        ImGui::SameLine();
        if (ImGui::Button("None"))
            SendNotifiction("I am the Title ! %s", "and I am the content", ImGuiToastType_None, 3.f);
        ImGui::SameLine();

        if (ImGui::Button("Show Demo"))
            isDemoOpened = true;
    }
    ImGui::End();
}

void EditorManager::HandleNotifications()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f); // Round borders
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f)); // Background color
    ImGui::RenderNotifications(); // <-- Here we render all notifications
    ImGui::PopStyleVar(1); // Don't forget to Pop()
    ImGui::PopStyleColor(1);
}

//Display time in seconds
void EditorManager::SendNotifiction(const char* title, const char* content, ImGuiToastType_ type, float displayTime)
{
    ImGui::InsertNotification({ type, displayTime, title, content});
}