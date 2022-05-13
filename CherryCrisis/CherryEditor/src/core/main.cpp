
#include "cherry_header.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "tahoma.h"
#include "ImGuizmo.h"
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 

#include "core/editor_manager.hpp"
#include "core/imgui_style.hpp"

#include "stb_image.h"
#include "input_manager.hpp"
#include "time_manager.hpp"

#include <iostream>
#include <crtdbg.h>

#include "utils.hpp"

void HideCursor(void* window) 
{
    GLFWwindow* castedWindow = (GLFWwindow*) window;
    glfwSetInputMode(castedWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void ShowCursor(void* window)
{
    GLFWwindow* castedWindow = (GLFWwindow*)window;
    glfwSetInputMode(castedWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
    for (int i = 0; i < count; i++) 
        EditorNotifications::ResourceImporting(CopyFile(paths[i], "Assets/"));
}

int main(int argc, char** argv)
{
    // Check for leak
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    int screenWidth = 1200;
    int screenHeight = 1000;

    if (!glfwInit())
    {
        printf("glfwInit failed\n");
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Cherry Crisis Engine", nullptr, nullptr);
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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    SetDarkStyle();
    ImGui_ImplOpenGL3_Init("#version 330");
    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 17.f, &font_cfg);
    ImGui::MergeIconsWithLatestFont(16.f, false);

    std::string projectPath = ""; 
    if (argc > 1) 
        projectPath = argv[1];

    Engine engine{};
    EditorManager editor{projectPath};

    glfwSetWindowUserPointer(window, &editor);
    glfwSetKeyCallback(window, [](GLFWwindow* w, int k, int s, int a, int m)
    {
        InputManager::KeyCallback(w, k, s, a, m);
    });
    glfwSetWindowFocusCallback(window, [](GLFWwindow* w, int i)
    {
        static_cast<EditorManager*>(glfwGetWindowUserPointer(w))->FocusCallback(w, i);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* w, double x, double y)
    {
        InputManager::MouseWheelCallback(w, x, y);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y)
    {
        InputManager::MousePosCallback(w, x, y);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int k, int a, int m)
    {
        InputManager::MouseClickCallback(w, k, a, m);
    });
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    glfwSetDropCallback(window, drop_callback);

    InputManager::GetInstance()->HideCursor = HideCursor;
    InputManager::GetInstance()->ShowCursor = ShowCursor;
    Engine::window_handle = window;

    Serializer::UnserializeInputs();

    editor.LinkEngine(&engine);
    engine.window_handle = window;

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // TODO: Change to pre-compiled image
    stbi_set_flip_vertically_on_load(false);
    GLFWimage icon[1];
    icon[0].pixels = stbi_load("internal/icon.png", &icon[0].width, &icon[0].height, NULL, 4);
    glfwSetWindowIcon(window, 1 , icon);
    stbi_image_free(icon[0].pixels);
    ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
    //-----------------------------------

    int isSceneFocused = false;

    while (glfwWindowShouldClose(window) == false)
    {
        InputManager::UpdateKeys();
        TimeManager::GetInstance()->Update((float)glfwGetTime());
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 1.f);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
        
        editor.DisplayEditorUI(window);

        engine.TickEngine();

        if (engine.m_isPlaying && !engine.m_isPaused)
        {
            InputManager::PushContext("User Context");
            engine.Tick();
            InputManager::PopContext();
        }

        glfwSwapBuffers(window);

        if (!isSceneFocused)
        {
            ImGui::SetWindowFocus("Scene");
            isSceneFocused = true;
        }
    }
    //Save editor file
    Serializer::SerializeEditor();

    io.Fonts->ClearFonts();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
