//
// Created by ling on 23-5-22.
//

#ifndef GUIMEMORYPLUG_MAINGUI_H
#define GUIMEMORYPLUG_MAINGUI_H

bool MainWindow(int key);
//版本号
#define VERSION_CODE 1

/**
 * 命令代码
 */


struct SELECT_TARGET_DATA {
    int version;    //协议版本
    int proc;       //监听端口号
};

/**
 * 搜索设备响应数据包
 */
struct PUSH_TARGET_DATA {
    char name[128];     //设备名称
};

#include <unistd.h>
#include <limits.h>

#if WINDOWS_BUILD

#include "windows.h"

#endif

        std::string

GetExecutablePath();

#endif //GUIMEMORYPLUG_MAINGUI_H
