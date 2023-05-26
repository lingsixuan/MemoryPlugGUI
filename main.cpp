// Copyright (c) 2023 驻魂圣使
// Licensed under the GPL-3.0 license

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <atomic>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <functional>
#include "GUI/MainGUI.h"
#include <memory>

#include "GUI/WindowManager.h"
#include "Window/MainWindowObject.h"


#ifdef WINDOWS_BUILD
#include <windows.h>
#endif


int main() {
#ifdef WINDOWS_BUILD
    // 设置控制台输出编码为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif
    WindowInit((ling::WindowDataObject *) new ling::MainWindowObject);
    return 0;
}
