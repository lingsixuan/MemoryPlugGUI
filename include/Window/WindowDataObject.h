//
// Created by ling on 23-5-22.
//

#ifndef GUIMEMORYPLUG_WINDOWDATAOBJECT_H
#define GUIMEMORYPLUG_WINDOWDATAOBJECT_H

#include <functional>
#include "imgui.h"
#include <memory>
namespace ling {
    /**
     * 窗口基类
     */
    class WindowDataObject {

    private:

    protected:
        /**
         * 添加一个窗口
         */
        static void addWindow(ling::WindowDataObject *window);

    public:

        WindowDataObject();

        virtual ~WindowDataObject();

        /**
         * 绘制函数
         * @param key
         * @return
         */
        virtual bool DrawWindow(int key);
        /**
         * 开始绘制
         * @return 如果返回false，则当前页面不会被加入窗口栈
         */
        virtual bool start(std::shared_ptr<WindowDataObject> ptr);

        /**
         * 停止绘制
         */
        virtual void stop();
    };

} // ling

#endif //GUIMEMORYPLUG_WINDOWDATAOBJECT_H
