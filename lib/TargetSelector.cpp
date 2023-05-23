//
// Created by ling on 23-5-23.
//

#include <iostream>
#include <thread>

#if WINDOWS_BUILD

#include <winsock.h>
#include <io.h>
#include <ws2tcpip.h>

#else

#include <pcap/socket.h>

#endif

#include <csignal>

#include "Window/TargetSelector.h"
#include "Window/MainWindowObject.h"

namespace ling {
    int TargetSelector::TargetSelectorNumber = 0;
    bool TargetSelector::isStartThread = false;
    float TargetSelector::scanningTime = 0;
    int TargetSelector::port = 10001;

    TargetSelector::TargetSelector() {

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
        ImGui::Begin("MemoryPlug GUI -- 设备管理",
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

        ImGui::InputInt("监听端口", &port, 1, 0, isStartThread ? ImGuiInputTextFlags_ReadOnly : 0);

        if (port > 65535 || port < 1) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "端口号%d超出范围：1-65535", port);
        }

        if (ImGui::Button("扫描设备")) {
            if (port < 65535 && port > 1) {
                startThread();
                ling::TargetSelector::scanningTime = 5;
            }
        }

        if (ling::TargetSelector::scanningTime > 0) {
            ImGui::Text("正在扫描...");
            ling::TargetSelector::scanningTime -= ImGui::GetIO().DeltaTime;
        }

        ImGui::End();
        return false;
    }

    void TargetSelector::stop() {
        WindowDataObject::stop();
        TargetSelectorNumber--;
    }

    void TargetSelector::startThread() {
        if (ling::TargetSelector::scanningTime > 0) {
            ling::MainWindowObject::addLog(DEBUG, "正在搜索，请稍候");
            return;
        }
        if (!isStartThread) {
            isStartThread = true;
            std::cout << "搜索线程启动" << std::endl;
            std::thread thread1([]() -> void {

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
                std::string message = "Hello, broadcast!";
                if (sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
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
            });
            thread1.detach();
        }
    }
} // ling