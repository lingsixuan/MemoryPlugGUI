// Copyright (c) 2023 驻魂圣使
// Licensed under the GPL-3.0 license

#ifndef GUIMEMORYPLUG_SELECTPROCWINDOW_H
#define GUIMEMORYPLUG_SELECTPROCWINDOW_H

#include "WindowDataObject.h"

namespace ling {
    /**
     * 选择进程窗口
     */
    class SelectProcWindow : WindowDataObject {
    protected:
        static int SelectNumber;
        char windowTitle[512]{};

    public:
        SelectProcWindow();

        ~SelectProcWindow() override;

        bool DrawWindow(int key) override;

        bool start(std::shared_ptr<WindowDataObject> ptr) override;

        void stop() override;
    };
}


#endif //GUIMEMORYPLUG_SELECTPROCWINDOW_H
