//
// Created by ling on 23-5-22.
//  窗口管理器
//


#include <memory>
#include "GUI/WindowManager.h"
#include "Window/WindowDataObject.h"
#include "GUI/MainGUI.h"

std::unordered_map<int, std::shared_ptr<ling::WindowDataObject>> WindowList;
int WindowIndex = 0;

// GLFW 窗口
GLFWwindow *g_Window = nullptr;

//窗口读写自旋锁
std::atomic_flag WindowsReadWriteLock;

void addWindowGUI(ling::WindowDataObject *window) {
    while (!WindowsReadWriteLock.test_and_set(std::memory_order_release));
    std::shared_ptr<ling::WindowDataObject> ptr(window);
    if (window->start(ptr))
        WindowList[WindowIndex++] = ptr;
    WindowsReadWriteLock.clear(std::memory_order_release);
}

void WindowInit(ling::WindowDataObject *mainWindow) {
    WindowInit(std::shared_ptr<ling::WindowDataObject>(mainWindow));
}

void WindowInit(std::shared_ptr<ling::WindowDataObject> mainWindow) {

    ImGui::CreateContext();

    std::string path = GetExecutablePath();
    if (path.empty()) {
        std::cerr << "error：获取可执行文件路径出错！" << std::endl;
        std::terminate();
    }

    char *pathChar = new char[path.length() + 1];
    strcpy(pathChar, path.c_str());
    for (unsigned long i = path.length(); i > 0; i--) {
        if (pathChar[i] == '\\' || pathChar[i] == '/') {
            pathChar[i] = '\0';
            break;
        }
    }
    std::string fontPath = pathChar;
    fontPath += "/font/heng.ttf";
    delete[] pathChar;

    ImGuiIO &io = ImGui::GetIO();
    ImFont *font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 20.0f, nullptr,
                                                io.Fonts->GetGlyphRangesChineseFull());
    // 初始化 GLFW
    if (!glfwInit()) {
        return;
    }
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // 窗口不可调整大小
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // 去除窗口边框和标题栏
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE); // 窗口最大化
    // 获取屏幕的分辨率
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    // 创建窗口
    g_Window = glfwCreateWindow(mode->width, mode->height, "ImGUI测试", nullptr, nullptr);
    if (!g_Window) {
        glfwTerminate();
        return;
    }

    // 创建 OpenGL 上下文
    glfwMakeContextCurrent(g_Window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return;
    }

    // 设置 ImGui 渲染器
    ImGui_ImplGlfw_InitForOpenGL(g_Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 设置 ImGui 配置
    ImGui::StyleColorsDark();
    if (!mainWindow->start(mainWindow)) {
        std::cerr << "无法实例化MainWindow！" << std::endl;
        std::terminate();
    }
    WindowList[WindowIndex++] = mainWindow;
    // 主循环
    while (!glfwWindowShouldClose(g_Window)) {
        glfwPollEvents();

        // 开始 ImGui 帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT);

        // 绘制 ImGui 窗口
        //DrawImGuiWindow();
        while (!WindowsReadWriteLock.test_and_set(std::memory_order_release));
        for (auto it = WindowList.begin(); it != WindowList.end();) {
            if (it->second->DrawWindow(it->first)) {
                std::cout << it->first << "号窗口被关闭" << std::endl;
                it->second->stop();
                it = WindowList.erase(it);
            } else {
                ++it;
            }
        }
        WindowsReadWriteLock.clear(std::memory_order_release);

        // 渲染 ImGui 绘制命令
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 交换缓冲区并处理输入
        glfwSwapBuffers(g_Window);
    }

    // 清理 ImGui 相关资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // 清理 GLFW 相关资源
    glfwTerminate();
}