//
// Created by ling on 23-5-22.
//

#include <cstdio>
#include <iostream>
#include "Window/WindowDataObject.h"

void addWindowGUI(ling::WindowDataObject *window);

namespace ling {
    WindowDataObject::WindowDataObject() = default;

    WindowDataObject::~WindowDataObject() = default;

    bool WindowDataObject::DrawWindow(int key) {
        ImGui::Begin("MemoryPlug GUI -- 提示",
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

        ImGui::TextColored(ImVec4(1, 1, 1, 1), "您需要覆写WindowDataObject的DrawWindow函数！");

        ImGui::End();
        return false;
    }

    void WindowDataObject::addWindow(ling::WindowDataObject *window) {
        addWindowGUI(window);
    }

    bool WindowDataObject::start(std::shared_ptr<WindowDataObject> ptr) {
        return true;
    }

    void WindowDataObject::stop() {

    }
} // ling