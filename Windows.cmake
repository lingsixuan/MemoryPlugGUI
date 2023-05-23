cmake_minimum_required(VERSION 3.24)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_CXX_STANDARD 17)
set(IS_WINDOWS TRUE)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc-posix)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++-posix)
# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_SYSROOT /usr/x86_64-w64-mingw32)  # 或者 /usr/i686-w64-mingw32，根据你的架构选择

include_directories(
        /usr/x86_64-w64-mingw32/include/

)
set(glfw3 /usr/x86_64-w64-mingw32/lib/libglfw3.a)
set(glew32 /usr/x86_64-w64-mingw32/lib/libglew32.dll.a)

add_compile_definitions(WINDOWS_BUILD)