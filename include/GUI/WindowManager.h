//
// Created by ling on 23-5-22.
//

#ifndef GUIMEMORYPLUG_WINDOWMANAGER_H
#define GUIMEMORYPLUG_WINDOWMANAGER_H

#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <atomic>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Window/WindowDataObject.h"

void WindowInit(std::shared_ptr<ling::WindowDataObject> mainWindow);

void WindowInit(ling::WindowDataObject *mainWindow);

void addWindowGUI(ling::WindowDataObject *window);


#endif //GUIMEMORYPLUG_WINDOWMANAGER_H
