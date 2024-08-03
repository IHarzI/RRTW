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

        const char* ApplicationName = "RTW";

        std::mutex MultithreadPixelWriteMutex;

        RTW_STATIC GlobalState& GetGlobalStateInstance() { RTW_STATIC GlobalState instance; return instance; }
    };

    RTW_STATIC GlobalState& GetRTWGlobalState() {
        return GlobalState::GetGlobalStateInstance();
    };
};