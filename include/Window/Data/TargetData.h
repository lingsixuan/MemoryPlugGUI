//
// Created by ling on 23-5-23.
//

#ifndef GUIMEMORYPLUG_TARGETDATA_H
#define GUIMEMORYPLUG_TARGETDATA_H

#include <string>

namespace ling {
    /**
     * 设备控制类
     */
    class TargetData {
    private:
        //设备IP地址
        std::string ip;
        //套接字描述符
        int fd = -1;
        //设备名称
        std::string name;
        //设备ID，由受控端随机生成
        //int id = -1;

    public:
        TargetData(const std::string &name, const std::string &ip);

        ~TargetData();

        [[nodiscard]] const std::string &getName() const;

        [[nodiscard]] const std::string &getIP() const;
    };

} // ling

#endif //GUIMEMORYPLUG_TARGETDATA_H
