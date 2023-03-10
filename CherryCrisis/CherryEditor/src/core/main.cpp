
#include <iostream>
#include <crtdbg.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include <stb_image.h>
#include <tahoma.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 
#include <glad/gl.h>

#include "input_manager.hpp"
#include "scene_manager.hpp"
#include "time_manager.hpp"
#include "csscripting_system.hpp"

#include "core/editor_manager.hpp"
#include "core/imgui_style.hpp"
#include "command.hpp"
#include "utils.hpp"

void HideCursor(void* window) 
{
    GLFWwindow* castedWindow = (GLFWwindow*) window;
    glfwSetInputMode(castedWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

void ShowCursor(void* window)
{
    GLFWwindow* castedWindow = (GLFWwindow*)window;
    glfwSetInputMode(castedWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    ImGui::GetIO().ConfigFlags = ImGui::GetIO().ConfigFlags & ~ImGuiConfigFlags_NoMouse;
}

float GetTime()
{
    return static_cast<float>(glfwGetTime());
}

void SetTime(float time)
{
    glfwSetTime(time);
}

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
    if (EditorManager* editor = static_cast<EditorManager*>(glfwGetWindowUserPointer(window)))
    {
        if (AssetBrowser* browser = editor->GetAssetBrowser())
        {
            std::string directoryPath = browser->GetCurrentDirectoryPath();

            for (int i = 0; i < count; i++)
            {
                EditorNotifications::ResourceImporting(CopyFile(paths[i], directoryPath.c_str()));
            }

            browser->QuerryBrowser();
        }
    }
}

int main(int argc, char** argv)
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

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

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
    //io.Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 17.f, &font_cfg);
    io.Fonts->AddFontFromFileTTF("Mockery.otf", 13.f);
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //ImGui::MergeIconsWithLatestFont(16.f, false);


    std::string projectPath = ""; 
    if (argc > 1) 
        projectPath = argv[1];

    if (projectPath.size() > 0)
        std::filesystem::current_path(projectPath);

#ifdef NDEBUG
    std::string solutionPath = projectPath + "/CherryScripting.sln /Build";
    callEx("open", "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\devenv", solutionPath.c_str());
#endif

    {
        Engine engine{};
        {
            EditorManager editor{ projectPath };

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

            glfwSetDropCallback(window, drop_callback);

            ImGui_ImplGlfw_InitForOpenGL(window, true);

            //Render first time loading screen
            if (1) {
                glfwPollEvents();

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glClearColor(0.f, 0.f, 0.f, 1.f);

                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                //EditorManager::HandleDocking();
                
                ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
                ImGui::SetNextWindowSize(io.DisplaySize);
                ImGui::Begin("###fkfndkf", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMouseInputs);
                ImGui::Text("Loading Project...");
                ImGui::End();

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                glfwSwapBuffers(window);
            }

            editor.SetPath();

            InputManager::GetInstance()->HideCursor = HideCursor;
            InputManager::GetInstance()->ShowCursor = ShowCursor; 
            
            TimeManager::GetInstance()->GetTime = GetTime;
            TimeManager::GetInstance()->SetTime = SetTime;

            Engine::window_handle = window;

            Serializer::UnserializeInputs();

            editor.LinkEngine(&engine);
            engine.window_handle = window;

            stbi_set_flip_vertically_on_load(false);
            GLFWimage icon[1];
            icon[0].pixels = stbi_load("internal/icon.png", &icon[0].width, &icon[0].height, NULL, 4);
            glfwSetWindowIcon(window, 1, icon);
            stbi_image_free(icon[0].pixels);
            //-----------------------------------
        
            ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

            int isSceneFocused = false;
            glfwSwapInterval(0);

            SceneManager::GetInstance()->m_lateHierarchyRefresh.Bind(&EditorManager::RefreshHierarchy, &editor);
            SceneManager::GetInstance()->m_sceneChanged.Bind(&EntitySelector::ClearEx, &editor.m_entitySelector);
            SceneManager::GetInstance()->m_sceneChanged.Bind(&EditorManager::InvalidateCellDisplayer, &editor);

            while (glfwWindowShouldClose(window) == false)
            {
                InputManager::UpdateKeys();
                TimeManager::GetInstance()->Update((float)glfwGetTime());
                glfwPollEvents();

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glClearColor(0.f, 0.f, 0.f, 1.f);

                SceneManager::GetInstance()->m_currentScene->m_UIContext.ResetHoveredValues();

                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                ImGuizmo::BeginFrame();

                engine.TickEngine();

                if (Engine::isPlaying && !Engine::isPaused)
                {
                    InputManager::PushContext("User Context");
                    engine.Tick();
                    InputManager::PopContext();
                }

                if (!isSceneFocused)
                {
                    ImGui::SetWindowFocus("Scene");
                    isSceneFocused = true;
                }

                editor.DisplayEditorUI(window);
                glfwSwapBuffers(window);
                engine.EndFrame();
                editor.CheckForHierarchyRefresh();
                CsScriptingSystem::GetInstance()->onReload.Invoke();
            }
            //Save editor file
            
            Serializer::SerializeEditor(editor.GetPack());

            io.Fonts->ClearFonts();

            // Cleanup
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        } 
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
