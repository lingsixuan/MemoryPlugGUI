// Copyright (c) 2023 驻魂圣使
// Licensed under the GPL-3.0 license

#include <iostream>
#include <cmath>
#include <csignal>
#include "Window/MainWindowObject.h"
#include "Window/SelectProcWindow.h"
#include "Window/TargetSelector.h"
#include "Window/Class/TCP.h"

namespace ling {

    std::vector<std::shared_ptr<LogData>> MainWindowObject::logList;
    std::atomic_flag MainWindowObject::logLock;

    MainWindowObject::MainWindowObject() = default;

    MainWindowObject::~MainWindowObject() = default;

    bool MainWindowObject::DrawWindow(int key) {
        ImGui::Begin("MemoryPlug GUI -- 主菜单",
                     nullptr, /*ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |*/
                     ImGuiWindowFlags_MenuBar);


        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("文件")) {
                if (ImGui::MenuItem("打开", "")) {
                    addWindow((ling::WindowDataObject *) new ling::SelectProcWindow);
                }
                if (ImGui::MenuItem("保存", "")) {

                }
                if (ImGui::MenuItem("关闭", "")) {
                    delete ling::TCP::getTCP();
                    exit(0);
                    //mainWindowFlag.store(false, std::memory_order_release);
                    return true;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("运行")) {
                if (ImGui::MenuItem("设备")) {
                    addWindow((ling::WindowDataObject *) new ling::TargetSelector);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (ImGui::Button("扫描Server")) {
            this->hideTextTime = 2;
        }

        if (this->hideTextTime >= 0) {
            this->hideTextTime -= ImGui::GetIO().DeltaTime;
            ImGui::Text("未实现");
        }
        while (logLock.test_and_set(std::memory_order_release));
        for (auto it = logList.begin(); it != logList.end();) {
            if (it->get()->time < 0) {
                it = logList.erase(it);
                continue;
            }
            if (it->get()->v == ERROR)
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", it->get()->log.c_str());
            else
                ImGui::Text("%s", it->get()->log.c_str());
            it->get()->time -= ImGui::GetIO().DeltaTime;
            it++;
        }
        logLock.clear(std::memory_order_release);

        ImGui::End();
        return false;
    }

    void MainWindowObject::addLog(int v, const std::string &log) {
        while (logLock.test_and_set(std::memory_order_release));
        auto *p = new LogData;
        p->log = log;
        p->v = v;
        MainWindowObject::logList.push_back(std::shared_ptr<LogData>(p));
        logLock.clear(std::memory_order_release);
    }
}