//
// Created by ling on 23-5-23.
//

#include "Window/Data/TargetData.h"
#include "GUI/MainGUI.h"
#include <Window/Class/TCP.h>
#include <iostream>

namespace ling {
    std::unordered_map<int, std::vector<std::shared_ptr<EventData>> *> TargetData::eventList;
    std::mutex TargetData::eventListLock;

    TargetData::TargetData(const std::string &name, const std::string &ip, SOCKET fd) {
        this->name = name;
        this->ip = ip;
        this->fd = fd;
        this->ping.store(0, std::memory_order_release);
        this->pingSleepTime.store(0, std::memory_order_release);
        addEvent(API_CODE_PING, [=](int apiCode, const std::shared_ptr<char> &buff) {
            if (apiCode == API_CODE_PING) {
                // 获取当前时间点
                auto now = std::chrono::system_clock::now();
                // 将时间点转换为毫秒级时间戳
                auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
                auto timestamp = ms.time_since_epoch().count();
                this->pingSleepTime.store(0, std::memory_order_release);
                int64_t ti = *(int64_t *) buff.get();
                int64_t ti1 = timestamp;
                this->ping.store((ti1 - ti) / 2, std::memory_order_release);
            } else {
                std::cerr << "事件分发错误！" << std::endl;
            }
        });
    }

    TargetData::~TargetData() {

    }

    const std::string &TargetData::getName() const {
        return this->name;
    }

    const std::string &TargetData::getIP() const {
        return this->ip;
    }

    void TargetData::addEvent(int apiCode, const std::function<void(int, std::shared_ptr<char>)> &call) {
        eventListLock.lock();
        if (eventList.count(apiCode) == 0) {
            //如果TCP事件栈中没有这个事件，则添加一个二次分发的标准函数
            TCP::getTCP()->addEventHandler(
                    apiCode, [=](int apiCode, SOCKET ffd, const std::string &iip, const std::shared_ptr<char> &buff) {
                        eventCall(apiCode, ffd, iip, buff);
                    });
            eventList[apiCode] = new std::vector<std::shared_ptr<EventData>>;
        }
        auto event = std::make_shared<EventData>(fd, call);
        auto list = eventList[apiCode];
        for (auto it = list->begin(); it != list->end();) {
            if (it->get()->fd == fd) {
                eventListLock.unlock();
                return;
            }
            it++;
        }
        list->push_back(event);
        eventListLock.unlock();
    }

    void TargetData::removeEvent(int apiCode) {
        eventListLock.lock();
        if (eventList.count(apiCode) != 0) {
            auto list = eventList[apiCode];
            for (auto it = list->begin(); it != list->end();) {
                if (it->get()->fd == fd) {
                    list->erase(it);
                    eventListLock.unlock();
                    return;
                }
                it++;
            }
        }
        eventListLock.unlock();
    }

    void TargetData::eventCall(int apiCode, SOCKET fd, const std::string &ip, std::shared_ptr<char> buff) {
        eventListLock.lock();
        if (eventList.count(apiCode) == 1) {
            auto list = eventList[apiCode];
            for (auto it = list->begin(); it != list->end();) {
                if (it->get()->fd == fd) {
                    it->get()->call(apiCode, buff);
                }
                it++;
            }
        }
        eventListLock.unlock();
    }

    int TargetData::getPing() {
        return ping.load(std::memory_order_acquire);
    }

    void TargetData::pushPing() {
        // 获取当前时间点
        auto now = std::chrono::system_clock::now();
        // 将时间点转换为毫秒级时间戳
        auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        int64_t timestamp = ms.time_since_epoch().count();
        if (pingSleepTime.load(std::memory_order_acquire) != 0) {
            //上一次的ping还没有响应
            ping.store(ping.load(std::memory_order_acquire) + 1000, std::memory_order_release);
        } else {
            pingSleepTime.store(timestamp, std::memory_order_release);
            TCP::pushPack(fd, API_CODE_PING, &timestamp, sizeof(timestamp));
        }
    }
} // ling