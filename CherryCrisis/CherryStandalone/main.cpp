
#include "cherry_header.hpp"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 

#include "input_manager.hpp"
#include "time_manager.hpp"
#include "render_manager.hpp"

#include <iostream>

//#include <crtdbg.h>
#include "scene_manager.hpp"
#include "camera_component.hpp"
#include "framebuffer.hpp"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

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

void window_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    // Check for leak
   // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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
    //Callbacks
    {
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

        glfwSetWindowSizeCallback(window, window_size_callback);
    }

    InputManager::GetInstance()->HideCursor = HideCursor;
    InputManager::GetInstance()->ShowCursor = ShowCursor;
    
    Engine::window_handle = window;
    Serializer::UnserializeGame("master");
    InputManager::GetInstance()->SetCursorHidden();

    Framebuffer framebuffer;

    engine.LaunchStandalone();

    Serializer::UnserializeInputs();
    InputManager::GetInstance()->SetPollContext("User Context");
    InputManager::GetInstance()->PushContext("User Context");
    
    while (glfwWindowShouldClose(window) == false)
    {
        InputManager::GetInstance()->UpdateKeys();
        TimeManager::GetInstance()->Update((float)glfwGetTime());
        glfwPollEvents();

        glfwGetWindowSize(window, &framebuffer.colorTex.width, &framebuffer.colorTex.height);


        if (CameraComponent* cameraComp = CameraComponent::GetMainCamera())
        {
            Camera* cam = &cameraComp->m_camera;

            float aspect = (float)framebuffer.colorTex.width / (float)framebuffer.colorTex.height;
            cam->m_projectionMatrix = Matrix4::Perspective(cam->fovY, aspect, cam->near, cam->far);

            cam->Draw(framebuffer, 5);
        }

        engine.TickEngine();
        if (engine.isPlaying)
            engine.Tick();

        glfwSwapBuffers(window);
    }
    
    InputManager::GetInstance()->PopContext();

    // Terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}