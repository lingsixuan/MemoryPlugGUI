//
// Created by ling on 23-5-24.
//

#ifndef GUIMEMORYPLUG_TCP_H
#define GUIMEMORYPLUG_TCP_H

#include <mutex>
#include <functional>
#include <atomic>
#include <memory>

#if WINDOWS_BUILD

#include <winsock.h>
#include <io.h>
#include <ws2tcpip.h>

#else

#include <pcap/socket.h>

#endif

/**
 * 设备通讯类
 *
 * 本通讯框架对每个数据包的定义如下：
 * int32_t version      -->     通讯协议版本，协议没有前向兼容性。
 * int32_t API_CODE     -->     请求码/响应码
 * int64_t long dataSize    -->     数据包体大小
 * void [dataSize]  -->     数据包体
 * int32_t DATA_STOP        -->     标志数据包结束的魔数
 */
namespace ling {
    class TCP {

    private:
        TCP();

        SOCKET socketFd;

        static TCP *obj;
        //获取对象的锁
        static std::mutex getObjLock;
        //事件锁
        std::recursive_mutex eventLock;
        std::unordered_map<int, std::function<void(int api_code, SOCKET fd, const std::string &ip,
                                                   std::shared_ptr<char>)>> eventList;
        std::atomic<bool> isStart = false;

        void startClient(SOCKET fd, const std::string &ip);

    protected:
        /**
         * 启动数据接收器
         */
        [[noreturn]] void startMonitor(int port);

    public:

        ~TCP();

        /**
         * 获取本类单例
         * @return
         */
        static TCP *getTCP();

        /**
         * 发送数据
         * @return
         */
        static bool pushPack(SOCKET fd,int apiCode, void *buff, int64_t size);

        /**
         * 添加一个事件处理器
         * @param eventCode 事件代码，等价于API_CODE
         * @param call 事件回调
         */
        void addEventHandler(int eventCode, const std::function<void(int api_code, SOCKET fd, const std::string &ip,
                                                                     std::shared_ptr<char>)> &call);

        /**
         * 删除一个事件处理器
         * @param eventCode 注册时使用的API_CODE
         */
        void removeEventHandler(int eventCode);

        /**
         * 启动服务
         * @param port
         */
        void start(int port);
    };
}


#endif //GUIMEMORYPLUG_TCP_H
