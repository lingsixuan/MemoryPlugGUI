// Copyright (c) 2023 驻魂圣使
// Licensed under the GPL-3.0 license

#include <cstdio>
#include <iostream>
#include "Window/SelectProcWindow.h"

namespace ling {

    int SelectProcWindow::SelectNumber = 0;

    SelectProcWindow::SelectProcWindow() = default;

    SelectProcWindow::~SelectProcWindow() = default;

    bool SelectProcWindow::DrawWindow(int key) {
        ImGui::Begin("MemoryPlug GUI -- 选择进程",
                     nullptr, /*ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |*/
                     ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("文件")) {
                if (ImGui::MenuItem("关闭", "")) {
                    ImGui::EndMenu();
                    ImGui::EndMenuBar();
                    ImGui::End();
                    return true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::TextColored(ImVec4(1, 1, 1, 1), "Server未连接！");

        ImGui::End();
        return false;
    }

    bool SelectProcWindow::start(std::shared_ptr<WindowDataObject> ptr) {
        if (SelectNumber == 0) {
            SelectNumber++;
            sprintf(windowTitle, "MemoryPlug GUI -- 选择进程");
            return true;
        } else {
            std::cout << "选择进程：已经有一个窗口正在显示" << std::endl;
            return false;
        }
    }

    void SelectProcWindow::stop() {
        SelectNumber--;
    }
}