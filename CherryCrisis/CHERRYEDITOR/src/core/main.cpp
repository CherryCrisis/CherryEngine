
#include "CherryHeader.h"
#include "printer.hpp"

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

/*void KeyCallbackWrapper(GLFWwindow* w, int k, int s, int a, int m)
{
    void* userPointer = glfwGetWindowUserPointer(w);
    InputManager* IM = static_cast<InputManager*>(userPointer);

    IM->KeyCallback(w, k, s, a, m);
}
*/

int main()
{
    int screenWidth = 1200;
    int screenHeight = 1000;

    if (!glfwInit())
    {
        printf("glfwInit failed\n");
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "GLFW test", nullptr, nullptr);
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

    InputManager* IM = InputManager::instance();
    glfwSetWindowUserPointer(window, IM);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    SetDarkStyle();


    auto func = [](GLFWwindow* w, int k, int s, int a, int m)
    {
        static_cast<InputManager*>(glfwGetWindowUserPointer(w))->KeyCallback(w, k, s, a, m);
    };

    glfwSetKeyCallback(window, func);


    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 17.f, &font_cfg);
    ImGui::MergeIconsWithLatestFont(16.f, false);


    EditorManager editor{};
    Engine engine{};

    editor.LinkEngine(&engine);

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    while (glfwWindowShouldClose(window) == false)
    {
        InputManager::instance()->UpdateKeys();
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 1.f);

        editor.DisplayEditorUI(window);

        if (engine.isPlaying)
            engine.Tick();

        glfwSwapBuffers(window);
    }
}
