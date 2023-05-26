// Copyright (c) 2023 驻魂圣使
// Licensed under the GPL-3.0 license

#include <thread>
#include "Window/Class/TCP.h"
#include "GUI/MainGUI.h"
#include "Window/MainWindowObject.h"
#include <csignal>
#include <iostream>
#include <cstring>
#include <memory>


namespace ling {

    TCP *TCP::obj = nullptr;
    std::mutex TCP::getObjLock;

    TCP::TCP() {

    }

    TCP::~TCP() {
        close(socketFd);
    }

    TCP *TCP::getTCP() {
        getObjLock.lock();
        if (obj == nullptr) {
            obj = new TCP;
        }
        getObjLock.unlock();
        return obj;
    }

    [[noreturn]] void TCP::startMonitor(int port) {
#if WINDOWS_BUILD
        // 初始化Winsock库
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("初始化Windsock库失败！");
        }
#endif
        socketFd = socket(AF_INET, SOCK_STREAM, 0);
        if (socketFd <= 0) {
            throw std::runtime_error("创建服务失败！");
        }
        struct sockaddr_in myAddr{};
        memset(&myAddr, 0, sizeof(myAddr));
        myAddr.sin_family = AF_INET;    //IPv4
        myAddr.sin_port = htons(port);         //端口
        if (bind(socketFd, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0) {
            perror("绑定套接字错误");
            MainWindowObject::addLog(ERROR, "绑定套接字错误！");
            throw std::runtime_error("绑定套接字错误！");
        }
        if (listen(socketFd, 5) < 0) {
            throw std::runtime_error("注册监听失败！");
        }
        std::cout << "TCP服务启动成功" << std::endl;
        while (true) {
            struct sockaddr_in clientAddr{};
            socklen_t clientAddrLen = sizeof(clientAddr);
            SOCKET connFd = accept(socketFd, (struct sockaddr *) &clientAddr, &clientAddrLen);
            char *clientIP = inet_ntoa(clientAddr.sin_addr);
            if (connFd <= 0) {
                continue;
            }
            std::thread thread1([=]() {
                startClient(connFd, std::string(clientIP));
            });
            thread1.detach();
        }
#if WINDOWS_BUILD
        WSACleanup();
#endif
    }

    void TCP::addEventHandler(int eventCode, const std::function<void(int api_code, SOCKET fd, const std::string &ip,
                                                                      std::shared_ptr<char>)> &call) {
        eventLock.lock();
        eventList[eventCode] = call;
        eventLock.unlock();
    }

    void TCP::removeEventHandler(int eventCode) {
        eventLock.lock();
        eventList.erase(eventCode);
        eventLock.unlock();
    }

    void TCP::start(int port) {
        if (isStart.load(std::memory_order_release))
            throw std::runtime_error("事件监听器已经启动！");
        isStart.store(true, std::memory_order_release);
        std::thread thread1([=]() {
            try {
                startMonitor(port);
            } catch (std::runtime_error &e) {
                isStart.store(false, std::memory_order_release);
                std::cerr << "error：" << e.what() << std::endl;
            }
        });
        thread1.detach();
    }

    void TCP::startClient(SOCKET fd, const std::string &ip) {
        while (true) {
            int32_t version;    //协议版本号
            if (recv(fd, (char *) &version, sizeof(int32_t), 0) <= 0) {
                close(fd);
                return;
            }
            if (version != VERSION_CODE) {
                std::cout << fd << "号链接的协议版本(" << version << ")不支持！当前客户端支持" << VERSION_CODE << "版本的协议"
                          << std::endl;
                char log[512];
                sprintf(log, "%d号链接的协议版本(%d)不支持！当前客户端支持%d版本的协议", fd, version, VERSION_CODE);
                MainWindowObject::addLog(ERROR, log);
                return;
            }
            int32_t apiCode;        //请求码
            if (recv(fd, (char *) &apiCode, sizeof(int32_t), 0) <= 0) {
                close(fd);
                return;
            }
            int64_t dataSize;       //数据包体大小
            if (recv(fd, (char *) &dataSize, sizeof(int64_t), 0) <= 0) {
                close(fd);
                return;
            }
            std::shared_ptr<char> data = std::shared_ptr<char>(new char[dataSize]);
            if (recv(fd, data.get(), dataSize, 0) <= 0) {
                close(fd);
                return;
            }
            int32_t dataStop;
            if (recv(fd, (char *) &dataStop, sizeof(int32_t), 0) <= 0) {
                close(fd);
                return;
            }
            if (dataStop != DATA_STOP) {
                close(fd);
                std::cout << fd << "魔数格式错误！" << std::endl;
                return;
            }
            eventLock.lock();
            if (eventList.count(apiCode) == 1) {
                eventList[apiCode](apiCode, fd, ip, data);
            } else {
                std::cout << apiCode << "号事件不存在" << std::endl;
            }
            eventLock.unlock();
        }
    }

    bool TCP::pushPack(SOCKET fd, int apiCode, void *buff, int64_t size) {
        TCP_DATA_PACK data;
        data.version = VERSION_CODE;
        data.apiCode = apiCode;
        data.dataSize = size;
        data.dataPtr = (const char *) buff;
        data.dataStop = DATA_STOP;

        if (send(fd, (char *) &(data.version), sizeof(int32_t), 0) <= 0)
            return false;
        if (send(fd, (char *) &(data.apiCode), sizeof(int32_t), 0) <= 0)
            return false;
        if (send(fd, (char *) &(data.dataSize), sizeof(int64_t), 0) <= 0)
            return false;
        if (send(fd, data.dataPtr, data.dataSize, 0) <= 0)
            return false;
        if (send(fd, (char *) &(data.dataStop), sizeof(int32_t), 0) <= 0)
            return false;
        return true;
    }
}