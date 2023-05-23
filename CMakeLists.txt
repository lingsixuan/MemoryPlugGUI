cmake_minimum_required(VERSION 3.25)
project(GUIMemoryPlug)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)  #添加头文件搜索目录
include_directories(include/imgui)

add_executable(GUIMemoryPlug
        main.cpp
        include/imgui/imgui.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/backends/imgui_impl_glfw.cpp
        include/imgui/backends/imgui_impl_opengl3.cpp
        lib/WindowManager.cpp
        lib/MainGUI.cpp
        lib/WindowDataObject.cpp
        lib/MainWindowObject.cpp
        lib/SelectProcWindow.cpp
        lib/TargetSelector.cpp
        lib/TargetData.cpp
        )


if (${IS_WINDOWS})
    set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc-posix)
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++-posix)
    find_package(OpenGL REQUIRED)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    target_link_libraries(GUIMemoryPlug PRIVATE ${glfw3} ${glew32} OpenGL::GL -lkernel32 -luser32 -lws2_32)
else ()
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    target_link_libraries(GUIMemoryPlug PRIVATE glfw GLEW OpenGL::GL)
endif ()