// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing in One Weekend.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image(not multhithreaded).
//WIP.

#pragma once

#include "RTW_CORE.h"

#include <mutex>

namespace RTW
{
    struct GlobalState
    {
        bool IsRunning;
#ifdef  RTW_SYSTEM_WINDOWS
        HWND WindowHandle;
        HDC DeviceContext;
#endif //  RTW_SYSTEM_WINDOWS
        uint32 FrameBufferWidth;
        uint32 FrameBufferHeight;
        uint32* FrameBufferPixels;

        const char* ApplicationName = "RRTW";

        std::mutex MultithreadPixelWriteMutex;

        RTW_STATIC GlobalState& GetGlobalStateInstance() { RTW_STATIC GlobalState instance; return instance; }
    };

    RTW_STATIC GlobalState& GetRTWGlobalState() {
        return GlobalState::GetGlobalStateInstance();
    };
};