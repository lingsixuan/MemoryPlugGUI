// Copyright (c) 2023 驻魂圣使
// Licensed under the GPL-3.0 license

#include <iostream>
#include <thread>
#include <GUI/MainGUI.h>

#if WINDOWS_BUILD

#include <winsock.h>
#include <io.h>
#include <ws2tcpip.h>

#else

#include <pcap/socket.h>

#endif

#include <csignal>
#include <atomic>
#include <random>

#include "Window/TargetSelector.h"
#include "Window/MainWindowObject.h"
#include "Window/Class/TCP.h"

namespace ling {
    int TargetSelector::TargetSelectorNumber = 0;
    bool TargetSelector::isStartThread = false;
    float TargetSelector::scanningTime = 0;
    int TargetSelector::port = 10001;
    std::atomic<bool> TargetSelector::selectProcFlag(true);
    std::unordered_map<std::string, std::shared_ptr<ling::TargetData>> TargetSelector::targetMap;
    std::atomic_flag TargetSelector::targetMapLock;
    float TargetSelector::noThreadTime = 0;
    float TargetSelector::startThreadTime = 0;
    float TargetSelector::pingTime;

    TargetSelector::TargetSelector() {
        // 创建随机数引擎
        std::random_device rd;
        std::mt19937 gen(rd());
        // 创建随机数分布
        std::uniform_int_distribution<int> distribution(5000, 60000);
        // 生成随机数
        port = distribution(gen);
    }

    TargetSelector::~TargetSelector() {

    }

    bool TargetSelector::start(std::shared_ptr<WindowDataObject> ptr) {
        if (!WindowDataObject::start(ptr))
            return false;
        if (TargetSelectorNumber == 0) {
            TargetSelectorNumber++;
            return true;
        } else {
            return false;
        }
    }

    bool TargetSelector::DrawWindow(int key) {
        ImGui::Begin("MemoryPlug GUI -- 设备管理器",
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

        if (ling::TargetSelector::pingTime <= 0) {
            pingTime = 1;
            for (auto it = targetMap.begin(); it != targetMap.end();) {
                it->second->pushPing();
                it++;
            }
        } else {
            pingTime -= ImGui::GetIO().DeltaTime;
        }

        ImGui::InputInt("监听端口", &port, 1, 0, isStartThread ? ImGuiInputTextFlags_ReadOnly : 0);

        if (port > 65535 || port < 1) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "端口号%d超出范围：1-65535", port);
        }

        if (ImGui::Button("启动监听器")) {
            if (port < 65535 && port > 1) {
                startThread();
            }
        }

        if (startThreadTime > 0) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "监听器已经启动");
            startThreadTime -= ImGui::GetIO().DeltaTime;
        }

        if (isStartThread && startThreadTime <= 0) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 1, 1, 1), "监听器工作中");
            startThreadTime -= ImGui::GetIO().DeltaTime;
        }

        if (ImGui::Button("扫描设备")) {
            if (!isStartThread) {
                noThreadTime = 5;
            } else {
                targetMap.clear();
                if (ling::TargetSelector::scanningTime > 0) {
                    ling::MainWindowObject::addLog(DEBUG, "正在搜索，请稍候");
                }
                ling::TargetSelector::scanningTime = 5;
                TargetSelector::pushBroadcastAddress();
            }
        }

        if (noThreadTime > 0) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "请先启动监听器");
            ling::TargetSelector::noThreadTime -= ImGui::GetIO().DeltaTime;
        }

        if (ling::TargetSelector::scanningTime > 0) {
            ImGui::SameLine();
            ImGui::Text("正在扫描...请保持设备处于亮屏状态");
            ling::TargetSelector::scanningTime -= ImGui::GetIO().DeltaTime;
        }
        while (!targetMapLock.test_and_set(std::memory_order_release));
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "设备列表");
        ImGui::BeginChild("Scrolling");
        for (auto it = targetMap.begin(); it != targetMap.end();) {
            ImGui::Text("设备名称：%-15.15s IP：%s\tping：%-4.4d ms", it->second->getName().c_str(),
                        it->second->getIP().c_str(), it->second->getPing());
            ImGui::SameLine();
            ImGui::Button("连接");
            it++;
        }
        ImGui::EndChild();
        targetMapLock.clear(std::memory_order_release);

        ImGui::End();
        return false;
    }

    void TargetSelector::stop() {
        WindowDataObject::stop();
        TargetSelectorNumber--;
    }

    void TargetSelector::startThread() {
        if (!isStartThread) {
            isStartThread = true;
            noThreadTime = 0;
            std::cout << "搜索线程启动" << std::endl;
            std::thread thread1([=]() -> void {
                TCP *tcp = TCP::getTCP();
                tcp->addEventHandler(API_CODE_PUSH_TARGET_DATA,
                                     [](int apiCode, SOCKET fd, const std::string &ip,
                                        const std::shared_ptr<char> &ptr) {
                                         if (apiCode == API_CODE_PUSH_TARGET_DATA) {
                                             char *p = ptr.get();
                                             if (targetMap.count(ip) == 0)
                                                 targetMap[ip] = std::make_shared<TargetData>(p, ip, fd);
                                         }
                                     });
                tcp->start(port);
            });
            thread1.detach();
        } else {
            startThreadTime = 5;
        }
    }

    void TargetSelector::pushBroadcastAddress() {
#if WINDOWS_BUILD
        // 初始化Winsock库
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "初始化Winsock库失败" << std::endl;
            return;
        }
#endif

        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock == -1) {
            std::cerr << "创建套接字失败" << std::endl;
            MainWindowObject::addLog(ERROR, "创建套接字失败");
            return;
        }

        // 设置套接字选项，允许广播
#if WINDOWS_BUILD
        int enableBroadcast = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *) &enableBroadcast,
                       sizeof(enableBroadcast)) == SOCKET_ERROR) {
            std::cerr << "设置套接字选项失败: " << WSAGetLastError() << std::endl;
            MainWindowObject::addLog(ERROR, "设置套接字选项失败");
            closesocket(sock);
            return;
        }
#else
        int enableBroadcast = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &enableBroadcast, sizeof(enableBroadcast)) == -1) {
            std::cerr << "设置套接字选项失败" << std::endl;
            MainWindowObject::addLog(ERROR, "设置套接字选项失败");
            close(sock);
            return;
        }
#endif

        // 构建广播地址
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(10086);
        if (inet_pton(AF_INET, "255.255.255.255", &(addr.sin_addr)) <= 0) {
            std::cerr << "广播地址无效" << std::endl;
            MainWindowObject::addLog(ERROR, "广播地址无效");
            close(sock);
            return;
        }

        // 发送广播消息
        //std::string message = "Hello, broadcast!";
        SELECT_TARGET_DATA data{};
        data.proc = port;
        data.version = VERSION_CODE;
        if (sendto(sock, (const char *) &data, sizeof(data), 0, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
            std::cerr << "发送广播消息失败" << std::endl;
            MainWindowObject::addLog(ERROR, "发送广播消息失败");
            close(sock);
            return;
        }

        // 关闭套接字
        close(sock);
#if WINDOWS_BUILD
        WSACleanup();
#endif
    }
} // ling