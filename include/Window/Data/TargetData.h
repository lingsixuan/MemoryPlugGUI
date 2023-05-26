//
// Created by ling on 23-5-23.
//

#ifndef GUIMEMORYPLUG_TARGETDATA_H
#define GUIMEMORYPLUG_TARGETDATA_H

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>
#include <atomic>

#if WINDOWS_BUILD
#include <winsock.h>
#include <io.h>
#include <ws2tcpip.h>
#else

#include <pcap/socket.h>

#endif

namespace ling {

    struct EventData {
        SOCKET fd;
        std::function<void(int apiCode, std::shared_ptr<char> buff)> call;

        EventData(SOCKET fd, const std::function<void(int, std::shared_ptr<char>)> &call) : fd(fd), call(call) {

        }
    };

    /**
     * 设备控制类
     */
    class TargetData {
    private:
        //设备IP地址
        std::string ip;
        //套接字描述符
        SOCKET fd = -1;
        //设备名称
        std::string name;
        //延迟，单位毫秒
        std::atomic<int64_t> ping;
        //上次ping时间，当ping返回之后，此值为0
        std::atomic<int64_t> pingSleepTime;

        static std::unordered_map<int, std::vector<std::shared_ptr<EventData>> *> eventList;
        static std::mutex eventListLock;

        /**
         * 二次分发事件
         */
        static void eventCall(int apiCode, SOCKET fd, const std::string &ip, std::shared_ptr<char> buff);

    protected:

        void addEvent(int apiCode, const std::function<void(int apiCode, std::shared_ptr<char> buff)> &call);

        void removeEvent(int apiCode);

    public:
        TargetData(const std::string &name, const std::string &ip, SOCKET fd);

        ~TargetData();

        [[nodiscard]] const std::string &getName() const;

        [[nodiscard]] const std::string &getIP() const;

        int getPing();

        /**
         * 计算延迟
         */
        void pushPing();
    };

} // ling

#endif //GUIMEMORYPLUG_TARGETDATA_H
