
#include "cherry_header.hpp"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 

#include "input_manager.hpp"
#include "time_manager.hpp"
#include "render_manager.hpp"

#include <iostream>

#include <crtdbg.h>
#include "scene_manager.hpp"

struct Context 
{
    InputManager* inputs;
};

void HideCursor(void* window)
{
    GLFWwindow* castedWindow = (GLFWwindow*)window;
    glfwSetInputMode(castedWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void ShowCursor(void* window)
{
    GLFWwindow* castedWindow = (GLFWwindow*)window;
    glfwSetInputMode(castedWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

int main()
{
    // Check for leak
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    Engine engine{};
    Context context;
    context.inputs = InputManager::GetInstance();

    glfwSetWindowUserPointer(window, &context);
    glfwSetKeyCallback(window, [](GLFWwindow* w, int k, int s, int a, int m)
        {
            static_cast<Context*>(glfwGetWindowUserPointer(w))->inputs->KeyCallback(w, k, s, a, m);
        });
    glfwSetScrollCallback(window, [](GLFWwindow* w, double x, double y)
        {
            static_cast<Context*>(glfwGetWindowUserPointer(w))->inputs->MouseWheelCallback(w, x, y);
        });
    glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y)
        {
            static_cast<Context*>(glfwGetWindowUserPointer(w))->inputs->MousePosCallback(w, x, y);
        });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int k, int a, int m)
        {
            static_cast<Context*>(glfwGetWindowUserPointer(w))->inputs->MouseClickCallback(w, k, a, m);
        });

    InputManager::GetInstance()->HideCursor = HideCursor;
    InputManager::GetInstance()->ShowCursor = ShowCursor;
    Engine::window_handle = window;

    engine.window_handle = window;
    std::shared_ptr<Scene> scn;
    scn = ResourceManager::GetInstance()->AddResource<Scene>("scene de ouf", false);
    SceneManager::GetInstance()->SetCurrentScene(scn);

    SceneManager::GetInstance()->m_currentScene->Unserialize("scene de ouf.cherry");

    while (glfwWindowShouldClose(window) == false)
    {
        InputManager::GetInstance()->UpdateKeys();
        TimeManager::GetInstance()->Update((float)glfwGetTime());
        glfwPollEvents();

        int x = 0;
        int y = 0;

        glfwGetWindowSize(window, &x, &y);
        RenderManager::DrawScene(x, y);

        engine.TickEngine();



        if (engine.isPlaying)
            engine.Tick();

        glfwSwapBuffers(window);
    }

    // Terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}