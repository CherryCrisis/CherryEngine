
#include "cherry_header.hpp"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 

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
    InputManager* IM = InputManager::GetInstance();
    Engine engine{};

    auto func = [](GLFWwindow* w, int k, int s, int a, int m)
    {
        static_cast<InputManager*>(glfwGetWindowUserPointer(w))->KeyCallback(w, k, s, a, m);
    };

    auto funcW = [](GLFWwindow* w, double x, double y)
    {
        static_cast<InputManager*>(glfwGetWindowUserPointer(w))->MouseWheelCallback(w, x, y);
    };

    glfwSetWindowUserPointer(window, IM);

    glfwSetKeyCallback(window, func);
    glfwSetScrollCallback(window, funcW);

    while (glfwWindowShouldClose(window) == false)
    {
        InputManager::GetInstance()->UpdateKeys();
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 1.f);

        if (engine.isPlaying)
            engine.Tick();

        glfwSwapBuffers(window);
    }
}