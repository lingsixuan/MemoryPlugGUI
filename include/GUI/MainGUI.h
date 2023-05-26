//
// Created by ling on 23-5-22.
//

#ifndef GUIMEMORYPLUG_MAINGUI_H
#define GUIMEMORYPLUG_MAINGUI_H

bool MainWindow(int key);
//数据包体结束
#define DATA_STOP 0x20030507
//版本号
#define VERSION_CODE 1
//搜索设备响应数据包
#define API_CODE_PUSH_TARGET_DATA 1
//ping
#define API_CODE_PING 2

/**
 * 命令代码
 */


struct SELECT_TARGET_DATA {
    int version;    //协议版本
    int proc;       //监听端口号
};


struct TCP_DATA_PACK {
    int32_t version{};    //协议版本
    int32_t apiCode{};    //请求码
    int64_t dataSize{};   //包体大小
    const char *dataPtr{};     //数据包内容
    int32_t dataStop = DATA_STOP;
};

#include <unistd.h>
#include <limits.h>

#if WINDOWS_BUILD

#include "windows.h"

#endif

std::string

GetExecutablePath();

#endif //GUIMEMORYPLUG_MAINGUI_H
