
#include "cherry_header.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "tahoma.h"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 

#include "core/editor_manager.hpp"
#include "core/imgui_style.hpp"

#include "stb_image.h"
#include "input_manager.hpp"

#include <iostream>

int main()
{
    int screenWidth = 1200;
    int screenHeight = 1000;

    if (!glfwInit())
    {
        printf("glfwInit failed\n");
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Cherry Crisis", nullptr, nullptr);
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
    {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }

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


    Engine engine{};
    EditorManager editor{};

    auto func = [](GLFWwindow* w, int k, int s, int a, int m)
    {
        static_cast<EditorManager*>(glfwGetWindowUserPointer(w))->inputs->KeyCallback(w, k, s, a, m);
    };

    auto funcF = [](GLFWwindow* w, int i)
    {
        static_cast<EditorManager*>(glfwGetWindowUserPointer(w))->FocusCallback(w, i);
    };

    auto funcW = [](GLFWwindow* w, double x, double y)
    {
        static_cast<EditorManager*>(glfwGetWindowUserPointer(w))->inputs->MouseWheelCallback(w, x, y);
    };

    auto funcCP = [](GLFWwindow* w, double x, double y)
    {
        static_cast<EditorManager*>(glfwGetWindowUserPointer(w))->inputs->MousePosCallback(w, x, y);
    };


    auto funcCC = [](GLFWwindow* w, int k, int a, int m)
    {
        static_cast<EditorManager*>(glfwGetWindowUserPointer(w))->inputs->MouseClickCallback(w, k, a, m);
    };

    glfwSetWindowUserPointer(window, &editor);

    glfwSetKeyCallback(window, func);
    glfwSetWindowFocusCallback(window, funcF);
    glfwSetScrollCallback(window, funcW);
    glfwSetCursorPosCallback(window, funcCP);
    glfwSetMouseButtonCallback(window, funcCC);

    ImGui_ImplGlfw_InitForOpenGL(window, true);

    editor.LinkEngine(&engine);

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


    //glfwRequestWindowAttention(window);
    stbi_set_flip_vertically_on_load(false);
    GLFWimage icon[1];
    icon[0].pixels = stbi_load("internal/icon.png", &icon[0].width, &icon[0].height, NULL, 4);
    glfwSetWindowIcon(window, 1 , icon);
    stbi_image_free(icon[0].pixels);

    while (glfwWindowShouldClose(window) == false)
    {
        InputManager::GetInstance()->UpdateKeys();
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 1.f);

        editor.DisplayEditorUI(window);

        engine.TickEngine();

        if (engine.isPlaying)
            engine.Tick();

        glfwSwapBuffers(window);
    }
}
