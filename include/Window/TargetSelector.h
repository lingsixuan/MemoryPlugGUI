//
// Created by ling on 23-5-23.
//

#ifndef GUIMEMORYPLUG_TARGETSELECTOR_H
#define GUIMEMORYPLUG_TARGETSELECTOR_H

#include "WindowDataObject.h"
#include "Window/Data/TargetData.h"
#include <memory>

namespace ling {
    /**
     * 设备管理界面
     * 由此类负责维护设备列表
     * 设备列表由受控端通过广播地址发送到10086端口，控制端需要先向10001端口广播搜索命令，才能触发受控端广播。
     * 广播发送的数据由实现定义，需要携带协议版本号。通讯协议不考虑前向兼容性。
     */
    class TargetSelector : WindowDataObject {
    private:
        static int TargetSelectorNumber;
        //搜索设备时间
        static float scanningTime;
        //端口号
        static int port;
        //设备列表
        static std::unordered_map<int, std::shared_ptr<ling::TargetData>> targetMap;
        //搜索线程是否已经启动
        static bool isStartThread;

    protected:
        /**
         * 启动设备监听线程
         */
        static void startThread();

    public:
        TargetSelector();

        ~TargetSelector() override;

        bool start(std::shared_ptr<WindowDataObject> ptr) override;

        void stop() override;

        bool DrawWindow(int key) override;
    };

} // ling

#endif //GUIMEMORYPLUG_TARGETSELECTOR_H
