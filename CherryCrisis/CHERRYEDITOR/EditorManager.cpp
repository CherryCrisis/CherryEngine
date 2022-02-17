
#include "EditorManager.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <string>

#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <tchar.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 

#include <iostream>

#include <CherryHeader.h>
#include "printer.hpp"

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
    int null = 0;
    if (!LoadTextureFromFile("../Internal/Icons/file_icon.png", &FileIcon, &null, &null))
    {
        std::cout << "failed to load file icon" << std::endl;
    }
    if (!LoadTextureFromFile("../Internal/Icons/folder_icon.png", &browserIcon, &null, &null))
    {
        std::cout << "failed to load folder icon" << std::endl;
    }
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

    currentDirectory = AssetPath;
}



void EditorManager::DisplayEditorUI(GLFWwindow* window) 
{
    HandleDocking();

    HandleBrowserWindow();
    HandleLogWindow();
    HandleToolsWindow();
    HandleMenuBar();
    HandleGameWindow();
    HandleEditorWindow();
    HandleInspectorWindow();
    HandleGraphWindow(window);

    HandleNotifications();

    if (isDemoOpened)
        ImGui::ShowDemoWindow(&isDemoOpened);

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

void EditorManager::HandleLogWindow() 
{
    if (!isLogOpened)
        return;

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Logs", &isLogOpened, ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        Debug* debug = Debug::GetInstance();
        for (const auto message : debug->GetLogs()) 
        {
            ImGui::Text(message.c_str());
        }
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
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows"))
        {
            if (ImGui::BeginMenu("Open"))
            {
                if (ImGui::MenuItem("Browser")) { isBrowserOpened = true; }
                if (ImGui::MenuItem("Hierarchy")) { isHierarchyOpened = true; }
                if (ImGui::MenuItem("Log")) { isLogOpened = true; }
                if (ImGui::MenuItem("Inspector")) { isInspectorOpened = true; }
                if (ImGui::MenuItem("Game")) { isGameOpened = true; }
                if (ImGui::MenuItem("Scene")) { isSceneOpened = true; }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    
}

void EditorManager::HandleToolsWindow() 
{
    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoDockingOverMe;
    ImGui::SetNextWindowClass(&window_class);
    ImGui::SetNextWindowSize(ImVec2(500, 50), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Tools", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        float size = ImGui::GetWindowHeight() / 2.f;
        ImGui::SameLine((ImGui::GetWindowContentRegionMax().x * .5f) - (size * 3 * .5f));


        if (ImGui::ImageButton(engine->isPlaying ? (ImTextureID)StopIcon : (ImTextureID)PlayIcon, ImVec2(size, size), { 0,0 }, { 1,1 }, 0))
        {
            if (engine->isPlaying)
            {
                engine->Stop();
            }
            else
            {
                engine->Launch();
            }
        } ImGui::SameLine();
        if (ImGui::ImageButton((ImTextureID)PauseIcon, ImVec2(size, size), { 0,0 }, { 1,1 }, 0))
        {

        } ImGui::SameLine();
        if (ImGui::ImageButton((ImTextureID)ReplayIcon, ImVec2(size, size), { 0,0 }, { 1,1 }, 0))
        {

        }
    }

    ImGui::End();
}

void EditorManager::HandleBrowserWindow() 
{
    if (!isBrowserOpened)
        return;


    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Browser", &isBrowserOpened, ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        if (currentDirectory != AssetPath)
            if (ImGui::Button("<-"))
            {
                currentDirectory = currentDirectory.parent_path();
            }
        std::filesystem::path path_iterator = currentDirectory;

        while (path_iterator != AssetPath.parent_path())
        {
            ImGui::SameLine(); ImGui::Text("/"); ImGui::SameLine();
            std::string name = path_iterator.filename().string();
            ImGui::Text(name.c_str());

            path_iterator = path_iterator.parent_path();
        }

        QuerryBrowser();
    }
    ImGui::End();
}

void EditorManager::HandleGameWindow(unsigned int fbo) 
{
    if (!isGameOpened)
        return;

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Game", &isGameOpened))
    {
        // Using a Child allow to fill all the space of the window.
        // It also alows customization
        ImGui::BeginChild("GameRender");
        // Get the size of the child (i.e. the whole draw size of the windows).
        ImVec2 wsize = ImGui::GetWindowSize();
        // Because I use the texture from OpenGL, I need to invert the V from the UV.
        ImGui::Image((ImTextureID)fbo, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();
    }
    ImGui::End();
}

void EditorManager::HandleEditorWindow(unsigned int fbo) 
{
    if (!isSceneOpened)
        return;

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Scene", &isSceneOpened))
    {
        // Using a Child allow to fill all the space of the window.
        // It also alows customization
        ImGui::BeginChild("SceneRender");
        // Get the size of the child (i.e. the whole draw size of the windows).
        ImVec2 wsize = ImGui::GetWindowSize();
        // Because I use the texture from OpenGL, I need to invert the V from the UV.
        ImGui::Image((ImTextureID)fbo, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();
    }
    ImGui::End();
}

void EditorManager::HandleInspectorWindow() 
{
    if (!isInspectorOpened)
        return;

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Inspector", &isInspectorOpened))
    {
        for (const auto comp : engine->behaviours)
        {
            std::string name = typeid(*comp).name();
            name = name.substr(name.find_first_of(" \t") + 1);

            ImGui::Text(name.c_str());
        }
    }
    ImGui::End();
}

void EditorManager::HandleGraphWindow(GLFWwindow* window)
{
    if (!isHierarchyOpened)
        return;

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
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
            engine->behaviours.push_back(printer);
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

unsigned int EditorManager::GetIconByExtension(const char* ext)
{
    return 0;
}

void EditorManager::QuerryBrowser() 
{
    int i = 0;

    namespace fs = std::filesystem;
    
    float width = ImGui::GetContentRegionAvail().x;
    
    static float padding = 16.f;
    static float thumbnailSize = 128.f;
    float cellSize = thumbnailSize + padding;
    int columnCount = (int)(width / cellSize);    if (columnCount < 1) columnCount = 1;
    
    ImGui::Columns(columnCount, 0, false);

    for (const fs::directory_entry& entry : fs::directory_iterator(currentDirectory))
    {
        std::string name = entry.path().filename().string();

        unsigned int icon = entry.is_directory() ? browserIcon : FileIcon;

        ImGui::ImageButton((void*)(intptr_t)icon, {thumbnailSize, thumbnailSize}, { 0,1 }, { 1, 0 });
        
        // Double Click Callback
        if (ImGui::IsItemHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) >= 2)
        {
            if (entry.is_directory()) 
            {
                currentDirectory /= entry.path().filename();
            }
        }

        ImGui::Text(name.c_str());

        ImGui::NextColumn();
        i++;
    }
    ImGui::Columns(1);
}