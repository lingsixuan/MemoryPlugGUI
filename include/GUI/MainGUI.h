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

/**
 * struct {
 *      int version;    //协议版本
 *      char[32] ip;    //ip地址
 *      int proc;       //监听端口号
 * }
 */
#define SELECT_TARGET_CODE 1    //搜索设备，受控端收到此命令将会发送自己的基本信息到GUI

#include <unistd.h>
#include <limits.h>

#if WINDOWS_BUILD

#include "windows.h"

#endif

std::string GetExecutablePath();

#endif //GUIMEMORYPLUG_MAINGUI_H
