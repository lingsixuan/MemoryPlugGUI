// Copyright (c) 2023 驻魂圣使
// Licensed under the GPL-3.0 license

#include <iostream>
#include "GUI/MainGUI.h"
#include "imgui.h"

//主窗口
bool MainWindow(int key) {
    return false;
}

std::string GetExecutablePath() {
#if WINDOWS_BUILD
    char buffer[MAX_PATH];
        DWORD len = GetModuleFileName(NULL, buffer, MAX_PATH);
        if (len != 0) {
            return std::string(buffer);
        }
        return ""; // 获取路径失败
#else
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::string(buffer);
    }
    return ""; // 获取路径失败
#endif
}