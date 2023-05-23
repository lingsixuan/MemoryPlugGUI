//
// Created by ling on 23-5-22.
//

#ifndef GUIMEMORYPLUG_MAINWINDOWOBJECT_H
#define GUIMEMORYPLUG_MAINWINDOWOBJECT_H

#include "WindowDataObject.h"

#define DEBUG 1
#define ERROR 2

namespace ling {
    struct LogData {
        int v;  //日志级别
        std::string log;
        float time = 5;
    };

    /**
    * 主窗口对象
    */
    class MainWindowObject : WindowDataObject {
    protected:
        float hideTextTime = 0;
        static std::vector<std::shared_ptr<LogData>> logList;
        static std::atomic_flag logLock;

    public:
        MainWindowObject();

        ~MainWindowObject() override;

        bool DrawWindow(int key) override;

        static void addLog(int v, const std::string &log);
    };

}


#endif //GUIMEMORYPLUG_MAINWINDOWOBJECT_H
