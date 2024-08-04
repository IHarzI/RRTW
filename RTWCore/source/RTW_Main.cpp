// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing in One Weekend.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image(not multhithreaded).
//WIP.

#include <string>
#include <filesystem>

#include "RTW_CORE.h"
#include "RayCamera.h"
#include "RayList.h"
#include "Sphere.h"
#include "RTW_Logger.h"

#include <stdint.h>
#include <stdio.h>

#include "RTW_Context.h"
#include "Containers/RTW_DynamicArray.h"
#include <thread>

#ifdef  RTW_SYSTEM_WINDOWS
LRESULT Win32WindowCallBack(
    HWND WindowHandle,
    UINT Message,
    WPARAM WParam,
    LPARAM LParam
)
{
    LRESULT Result = {};

    switch (Message)
    {
    case WM_DESTROY:
    case WM_CLOSE:
    {
        RTW::GetRTWGlobalState().IsRunning = false;
    } break;

    default:
    {
        Result = DefWindowProcA(WindowHandle, Message, WParam, LParam);
    } break;
    }

    return Result;
}

// For now main code is a little mess 
// TODO: Refactor main

int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd)
{
    auto& RTWGlobalState = RTW::GlobalState::GetGlobalStateInstance();
    RTWGlobalState.IsRunning = true;
    LARGE_INTEGER TimerFrequency = {};

    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = Win32WindowCallBack;
    WindowClass.hInstance = hInstance;
    WindowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
    WindowClass.lpszClassName = RTWGlobalState.ApplicationName;
    if (!RegisterClassA(&WindowClass))
    {
        RTW_ASSERT(false);
    }
    // Setup RTW render
    // -----------------
    RTW::RayCamera Camera(1900, 2);
    RTWGlobalState.FrameBufferWidth = Camera.GetImageWidth();
    RTWGlobalState.FrameBufferHeight = Camera.GetImageHeight();
    RTW::Containers::DynamicArray<UniqueMemoryHandle<RTW::RayObject>> ObjectList{ 50 };

    auto material_ground =  MakeSharedHandle<RTW::Materials::Lambertian>(RTW::Math::vec3(0.8, 0.8, 0.0));
    auto material_center =  MakeSharedHandle<RTW::Materials::Lambertian>(RTW::Math::vec3(0.1, 0.2, 0.5));
    auto material_left =    MakeSharedHandle<RTW::Materials::Dielectric>(1.5);
    auto material_bubble =  MakeSharedHandle<RTW::Materials::Dielectric>(1.0/1.5);
    auto material_right =   MakeSharedHandle<RTW::Materials::Metal>(RTW::Math::vec3(0.8, 0.6, 0.2), 1.f);

    ObjectList.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3(0.0, -100.5, -1.0), 100.0f, material_ground.Get()).RetrieveResourse());
    ObjectList.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3(0.0, 0.0, -1.2), 0.5f,      material_center.Get()).RetrieveResourse());
    ObjectList.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3(-1.0, 0.0, -1.0), 0.5f,     material_left.Get()).RetrieveResourse());
    ObjectList.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3(-1.0, 0.0, -1.0), 0.4f,     material_bubble.Get()).RetrieveResourse());
    ObjectList.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3(1.0, 0.0, -1.0), 0.5f,      material_right.Get()).RetrieveResourse());

    RTW::RayList World{ std::move(ObjectList) };
    Camera.setPerPixelSamples(50);
    Camera.setDepth(25);
    // -----------------

    RTWGlobalState.WindowHandle = CreateWindowExA(
        0,
        WindowClass.lpszClassName,
        RTWGlobalState.ApplicationName,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_THICKFRAME,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        Camera.GetImageWidth(),
        Camera.GetImageHeight(),
        NULL,
        NULL,
        hInstance,
        NULL);

#ifdef RTW_BUILD_DEBUG
    // Console enabling
    AllocConsole();
    AttachConsole(ATTACH_PARENT_PROCESS);
    freopen("CONOUT$", "w", stdout);
#endif // RTW_BUILD_DEBUG


    if (!RTWGlobalState.WindowHandle)
    {
        RTW_ASSERT(false);
    }

    // kind of hack fix to match client window surface size with framebuffer 
    {
        RECT rcClient, rcWind;
        POINT ptDiff;
        GetClientRect(RTWGlobalState.WindowHandle, &rcClient);
        GetWindowRect(RTWGlobalState.WindowHandle, &rcWind);
        ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
        ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
        MoveWindow(RTWGlobalState.WindowHandle, rcWind.left, rcWind.top, Camera.GetImageWidth() + ptDiff.x, Camera.GetImageHeight() + ptDiff.y, TRUE);
    }

    RTWGlobalState.DeviceContext = GetDC(RTWGlobalState.WindowHandle);

    {
        RTWGlobalState.FrameBufferWidth = Camera.GetImageWidth();
        RTWGlobalState.FrameBufferHeight = Camera.GetImageHeight();
        uint32* PixelsAllocated = (uint32*)malloc(sizeof(uint32) * RTWGlobalState.FrameBufferWidth *
            RTWGlobalState.FrameBufferHeight);
        RTWGlobalState.FrameBufferPixels = PixelsAllocated;
    }

    LARGE_INTEGER BeginTime = {};
    LARGE_INTEGER EndTime = {};
    RTW_ASSERT(QueryPerformanceCounter(&BeginTime));

    // START RTW RENDER
    // ----------------
    std::thread RenderThread([&]() {Camera.render(World); });
    // ----------------

    while (RTWGlobalState.IsRunning)
    {
        RTW_ASSERT(QueryPerformanceCounter(&EndTime));
        float32 FrameTime = float32(EndTime.QuadPart - BeginTime.QuadPart) / float32(TimerFrequency.QuadPart);
        BeginTime = EndTime;

        MSG Message = {};
        while (PeekMessageA(&Message, RTWGlobalState.WindowHandle, 0, 0, PM_REMOVE))
        {
            switch (Message.message)
            {
            case WM_QUIT:
            {
                RTWGlobalState.IsRunning = false;
            } break;

            default:
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            } break;
            }
        }

        RECT ClientRect = {};
        RTW_ASSERT(GetClientRect(RTWGlobalState.WindowHandle, &ClientRect));
        uint32 ClientWidth = ClientRect.right - ClientRect.left;
        uint32 ClientHeight = ClientRect.bottom - ClientRect.top;

        BITMAPINFO BitmapInfo = {};
        BitmapInfo.bmiHeader.biSize = sizeof(tagBITMAPINFOHEADER);
        BitmapInfo.bmiHeader.biWidth = RTWGlobalState.FrameBufferWidth;
        BitmapInfo.bmiHeader.biHeight = -RTWGlobalState.FrameBufferHeight;
        BitmapInfo.bmiHeader.biPlanes = 1;
        BitmapInfo.bmiHeader.biBitCount = 32;
        BitmapInfo.bmiHeader.biCompression = BI_RGB;

        RTW_ASSERT(StretchDIBits(RTWGlobalState.DeviceContext,
            0,
            0,
            ClientWidth,
            ClientHeight,
            0,
            0,
            RTWGlobalState.FrameBufferWidth,
            RTWGlobalState.FrameBufferHeight,
            RTWGlobalState.FrameBufferPixels,
            &BitmapInfo,
            DIB_RGB_COLORS,
            SRCCOPY));
    }
    RenderThread.join();
    RTW_INFO("RayTracing application finished execution.");
    return 0;
}

#else

int main()
{
	RTW::RayCamera Camera(900, 2);
	
	RTW::RayObject* ObjectList[2];
	ObjectList[0] = new RTW::Sphere(RTW::Math::vec3{ .0f,.0f,-1.f }, .5f);
	ObjectList[1] = new RTW::Sphere(RTW::Math::vec3(.0f, -100.5f, -1.f), 100.f);
	RTW::RayList World{ ObjectList, 2 };
	Camera.setPerPixelSamples(5);
	Camera.render(World);

	std::cout << "RayTracing application finished execution.\n";
}
#endif