// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include <string>
#include <filesystem>

#include "RTW_CORE.h"
#include "RayCamera.h"
#include "RayList.h"
#include "Sphere.h"
#include "RTW_Logger.h"
#include "Quad.h"
#include "ConstantMedium.h"

#include <stdint.h>
#include <stdio.h>

#include "RTW_Context.h"
#include "BVHNode.h"
#include "Containers/RTW_DynamicArray.h"
#include <thread>



enum SceneID : uint32
{
    SpheresParadise,
    Mars,
    PerlinNoise,
    QuadsRoom,
    SimpleLightBox, 
    CornelBox,
    CornelBoxSmoke
};

using namespace RTW;

void PopulateScene(RTW::RayList::ObjectList& world, RTW::RayList::ObjectList& lights, RTW::RayCamera& Camera, SceneID sceneID)
{
    switch (sceneID)
    {
    case SpheresParadise:
    {
        auto material_center = MakeSharedHandle<RTW::Materials::Lambertian>(RTW::Math::color(0.9, 0.05, 0.1));
        auto material_left = MakeSharedHandle<RTW::Materials::Dielectric>(1.5);
        auto material_bubble = MakeSharedHandle<RTW::Materials::Dielectric>(1.0 / 1.5);
        auto material_right = MakeSharedHandle<RTW::Materials::Metal>(RTW::Math::color(0.05, 0.85, 0.92), 0.56f);
        auto texture_cheker = MakeSharedHandle<RTW::Textures::CheckerTexture>(0.32, RTW::Math::color{ .1,.05,.2 }, RTW::Math::color{ .9,.9,.9 });
        auto material_ground = MakeSharedHandle<RTW::Materials::Lambertian>(texture_cheker.Get());

        world.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3(0.0, -1002.5, -1.0), 1000, material_ground.Get()).RetrieveResourse());
        world.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3(-4.0, 1, 0), 0.5, material_center.Get()).RetrieveResourse());
        world.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3(0, 1.0, 0), 1.2, material_left.Get()).RetrieveResourse());
        world.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3(0, 1.0, 0), 0.85, material_bubble.Get()).RetrieveResourse());
        world.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3(0, 1.0, 0), 0.35, material_left.Get()).RetrieveResourse());
        world.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3(4.0, 1.0, 0), 0.5, material_right.Get()).RetrieveResourse());
        // Spawn paradise of spheres
        {
            const int MaxSpheresLoop = 15;
            for (int a = -MaxSpheresLoop; a < MaxSpheresLoop; a++) {
                for (int b = -MaxSpheresLoop; b < MaxSpheresLoop; b++) {
                    auto choose_mat = RTW::Util::randomDouble();
                    RTW::Math::vec3 center(a + 0.9 * RTW::Util::randomDouble(), RTW::Util::randomDouble(-0.5, 3.0), b + 0.9 * RTW::Util::randomDouble());

                    if (RTW::Math::less_or_equal(center, RTW::Math::vec3{ 1, 2, 0.2 }) &&
                        RTW::Math::more_or_equal(center, RTW::Math::vec3{ -0.2,0.6,-0.2 }))
                    {
                        center += RTW::Math::vec3{ 1.5,0,1.5 };
                    }

                    if ((center - RTW::Math::vec3(4, 0.2, 0)).Lenght() > 0.9) {
                        SharedMemoryHandle<RTW_Material> sphere_material;

                        if (choose_mat < 0.8) {
                            // diffuse
                            auto albedo = RTW::Util::RandomColor(0, 1) * RTW::Util::RandomColor(0., 1.);
                            auto texture_cheker = MakeSharedHandle<RTW::Textures::CheckerTexture>(RTW::Util::randomDouble(0.04, 0.12), albedo, RTW::Math::color{ .05,.05,.05 });
                            sphere_material = rtw_new<RTW::Materials::Lambertian>(texture_cheker.Get());
                            if (RTW::Util::randomDouble() > 0.8)
                            {
                                center += RTW::Math::vec3{ -13.0,.0,.0 };
                                auto centerMoved = center + RTW::Math::vec3{ 0,(float32)RTW::Util::randomDouble(-.01, .01),0 };

                                world.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(center, centerMoved, RTW::Util::randomDouble(0.1, 0.3), std::move(sphere_material)).RetrieveResourse());
                            }
                            else
                            {
                                world.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(center, RTW::Util::randomDouble(0.12, 0.34), std::move(sphere_material)).RetrieveResourse());
                            }
                            continue;
                        }
                        else if (choose_mat < 0.95) {
                            // metal
                            auto albedo = RTW::Util::RandomColor(0.5, 1);
                            auto fuzz = RTW::Util::randomDouble(0, 0.5);
                            sphere_material = rtw_new<RTW::Materials::Metal>(albedo, fuzz);
                        }
                        else {
                            // glass
                            sphere_material = rtw_new<RTW::Materials::Dielectric>(1.5);
                        }
                        world.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(center, 0.2, std::move(sphere_material)).RetrieveResourse());
                    }
                }
            }
        };
    }
    break;
    case Mars:
        {
            auto marsTexture = MakeSharedHandle<RTW::Textures::ImageTexture>("Mars_2k.jpg");
            auto marsSurface = MakeSharedHandle<RTW::Materials::Lambertian>(marsTexture.Get());
            world.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3{ 0,0,0 }, 2, marsSurface.Get()).RetrieveResourse());

        }
    break;
    case PerlinNoise:
        {
            auto perlinNose = MakeSharedHandle<RTW::Textures::NoiseTexture>(10);
            auto noiseMaterial = MakeSharedHandle<RTW::Materials::Lambertian>(perlinNose.Get());
            world.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3{ 0,-1000,0 }, 1000, noiseMaterial.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<RTW::Sphere>(RTW::Math::vec3{ 0,2,0 }, 2, noiseMaterial.Get()).RetrieveResourse());

        }
    break;
    case QuadsRoom:
        {
            auto redMat = MakeSharedHandle<RTW::Materials::Lambertian>(RTW::Math::color{ 1.0,.0,.0 });
            auto greenMat = MakeSharedHandle<RTW::Materials::Lambertian>(RTW::Math::color{ .0,1.0,.0 });
            auto blueMat = MakeSharedHandle<RTW::Materials::Lambertian>(RTW::Math::color{ .0,.0,1.0 });
            auto whiteMat = MakeSharedHandle<RTW::Materials::Lambertian>(RTW::Math::color{ 0.98,.98,.98 });
            auto NoiseText = MakeSharedHandle<RTW::Textures::NoiseTexture>(15);
            auto NoiseText2 = MakeSharedHandle<RTW::Textures::NoiseTexture>(6);
            auto SolidText = MakeSharedHandle<RTW::Textures::SolidColor>(Math::color{ 0.2,0.3,0.77 });
            auto Checkerboard = MakeSharedHandle<RTW::Textures::CheckerTexture>(.8, NoiseText.Get(), NoiseText2.Get());
            auto CheckerBoardNoiseMat = MakeSharedHandle<RTW::Materials::Lambertian>(Checkerboard.Get());
            auto DiamondGlass = MakeSharedHandle<RTW::Materials::Dielectric>(2.417);
            auto Ethanol = MakeSharedHandle<RTW::Materials::Dielectric>(1.36);
            auto Ice = MakeSharedHandle<RTW::Materials::Dielectric>(1.31);

            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{-4, -4, 8},     Math::vec3{0,0,-16}, Math::vec3{0,8,0}, redMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{-4, -4, 0},    Math::vec3{ 8,0,0 }, Math::vec3{ 0,8,0 }, greenMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{4, -4,  8},     Math::vec3{ 0,0,-16 }, Math::vec3{ 0,8,0 }, blueMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{-4, 4, 8}   , Math::vec3{ 8,0,0 }, Math::vec3{ 0,0,-8 }, whiteMat.Get()).RetrieveResourse());
            //world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ -4, -4, 8 }, Math::vec3{ 8,0,0 }, Math::vec3{ 0,0,-8 }, 
            //    CheckerBoardNoiseMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Sphere>(Math::vec3{ 0, -1000, 0 },994, CheckerBoardNoiseMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Sphere>(Math::vec3{ 0, 0, 0 }, 2.5, DiamondGlass.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Sphere>(Math::vec3{ 0.3, -0.1, 0 }, 0.7, Ethanol.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Sphere>(Math::vec3{ -0.2, 0.3, -0.1 }, 0.3, Ice.Get()).RetrieveResourse());

            Camera.setVFov(80);
            Camera.SetViewPerspective({ 0,0,13 }, { 0,0,0 }, { 0,1,0 });
            Camera.SetFocus(0.7, 12);
            Camera.backgroundColor = { .002,.002,.01 };

        }
    break;
    case SimpleLightBox:
        {
            auto redMat = MakeSharedHandle<Materials::Lambertian>(Math::color{ 1.0,.0,.0 });
            auto greenMat = MakeSharedHandle<Materials::Lambertian>(Math::color{ .0,1.0,.0 });
            auto blueMat = MakeSharedHandle<Materials::Lambertian>(Math::color{ .0,.0,1.0 });
            auto whiteMat = MakeSharedHandle<Materials::Lambertian>(Math::color{ 0.98,.98,.98 });
            auto LightMat = MakeSharedHandle<Materials::DiffuseLight>(Math::color{ 10,2,10 });

            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ -4, -4, 8 }, Math::vec3{ 0,0,-16 }, Math::vec3{ 0,8,0 }, redMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ -1, 2, 1 }, Math::vec3{ 2,0,0 }, Math::vec3{ 0,2,0 }, LightMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ -4, -4, 0 }, Math::vec3{ 8,0,0 }, Math::vec3{ 0,8,0 }, greenMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ 4, -4,  8 }, Math::vec3{ 0,0,-16 }, Math::vec3{ 0,8,0 }, blueMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ -4, 4, 8 }, Math::vec3{ 8,0,0 }, Math::vec3{ 0,0,-8 }, whiteMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ -4, -4, 8 }, Math::vec3{ 8,0,0 }, Math::vec3{ 0,0,-8 }, whiteMat.Get()).RetrieveResourse());
            auto DiamondGlass = MakeSharedHandle<Materials::Dielectric>(2.417);
            world.EmplaceBack(MakeUniqueHandle<Sphere>(Math::vec3{ 0, 0, 0 }, 2.5, DiamondGlass.Get()).RetrieveResourse());

            Camera.setVFov(80);
            Camera.SetViewPerspective({ 0,0,13 }, { 0,0,0 }, { 0,1,0 });
            Camera.SetFocus(0.7, 12);
            Camera.backgroundColor = { .002,.002,.01 };

        }
    break;
    case CornelBox:
        {
            auto redMat = MakeSharedHandle<Materials::Lambertian>(Math::color{ 1.0,.0,.0 });
            auto greenMat = MakeSharedHandle<Materials::Lambertian>(Math::color{ .0,1.0,.0 });
            auto blueMat = MakeSharedHandle<Materials::Lambertian>(Math::color{ .0,.0,1.0 });
            auto whiteMat = MakeSharedHandle<Materials::Lambertian>(Math::color{ 0.98,.98,.98 });
            auto LightMat = MakeSharedHandle<Materials::DiffuseLight>(Math::color{ 4,3.7,3.9 } * 2.1);
            auto whiteMetal = MakeSharedHandle<Materials::Metal>(Math::color{ 0.98,.98,.98 },.47);

            auto Glass = MakeSharedHandle<RTW::Materials::Dielectric>(1.31);

            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ -4, -4, 8 }, Math::vec3{ 0,0,-16 }, Math::vec3{ 0,8,0 }, redMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ 1, 3.8, 3 }, Math::vec3{ -2,0,0 }, Math::vec3{ 0,0,2 }, LightMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ -4, -4, 0 }, Math::vec3{ 8,0,0 }, Math::vec3{ 0,8,0 }, greenMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ 4, -4,  8 }, Math::vec3{ 0,0,-16 }, Math::vec3{ 0,8,0 }, blueMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ -4, 4, 8 }, Math::vec3{ 8,0,0 }, Math::vec3{ 0,0,-8 }, whiteMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ -4, -4, 8 }, Math::vec3{ 8,0,0 }, Math::vec3{ 0,0,-8 }, whiteMat.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Sphere>(Math::vec3{ 0, 2, 0 }, 1.35f, Glass.Get()).RetrieveResourse());
            world.EmplaceBack(MakeUniqueHandle<Sphere>(Math::vec3{ 2, 2, 6 }, .2f, whiteMetal.Get()).RetrieveResourse());
            Math::vec3 a = { 1,-4,6 };
            Math::vec3 b = { -1,-2,4 };
            UniqueMemoryHandle<RayObject> box1 = ConstructBox(a, b, whiteMat.Get()).RetrieveResourse();
            box1 = MakeUniqueHandle<RotateYOperation>(box1.RetrieveResourse(), 27.0).RetrieveResourse();
            box1 = MakeUniqueHandle<TranslatedObject>(box1.RetrieveResourse(), Math::vec3{ -1,0,0 }).RetrieveResourse();
            world.EmplaceBack(box1.RetrieveResourse());
            a = { 1,-6,6 };
            b = { -1,0,4 };
            UniqueMemoryHandle<RayObject> box2 = ConstructBox(a, b, whiteMetal.Get()).RetrieveResourse();
            box2 = MakeUniqueHandle<RotateYOperation>(box2.RetrieveResourse(), 65.0).RetrieveResourse();
            box2 = MakeUniqueHandle<TranslatedObject>(box2.RetrieveResourse(), Math::vec3{ -3,0,0 }).RetrieveResourse();
            world.EmplaceBack(box2.RetrieveResourse());

            Camera.setVFov(80);
            Camera.SetViewPerspective({ 0,0,13 }, { 0,0,0 }, { 0,1,0 });
            Camera.SetFocus(0.7, 12);
            Camera.backgroundColor = { .002,.002,.01 };

        }
    break;
    case CornelBoxSmoke:
        {
        auto redMat = MakeSharedHandle<Materials::Lambertian>(Math::color{ 1.0,.0,.0 });
        auto greenMat = MakeSharedHandle<Materials::Lambertian>(Math::color{ .0,1.0,.0 });
        auto blueMat = MakeSharedHandle<Materials::Lambertian>(Math::color{ .0,.0,1.0 });
        auto whiteMat = MakeSharedHandle<Materials::Lambertian>(Math::color{ 0.98,.98,.98 });
        auto LightMat = MakeSharedHandle<Materials::DiffuseLight>(Math::color{ 4,3.7,3.9 } * 2.5);
        auto whiteMetal = MakeSharedHandle<Materials::Metal>(Math::color{ 0.98,.98,.98 }, .05);
        auto emptyMat = MakeSharedHandle<RTW_Material>();
        auto Glass = MakeSharedHandle<RTW::Materials::Dielectric>(1.51);

        world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ 555,0,0}, Math::vec3{ 0,0,555 }, Math::vec3{ 0,555,0 }, greenMat.Get()).RetrieveResourse());
        world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ 0,0,555 }, Math::vec3{ 0,0,-555}, Math::vec3{ 0,555,0 }, redMat.Get()).RetrieveResourse());
        world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{0,555,0}, Math::vec3{ 555,0,0 }, Math::vec3{ 0,0,555 }, whiteMat.Get()).RetrieveResourse());
        world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ 0,0,555}, Math::vec3{555,0,0}, Math::vec3{ 0,0,-555}, whiteMat.Get()).RetrieveResourse());
        world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ 555,0,555}, Math::vec3{ -555,0,0}, Math::vec3{ 0,555,0}, whiteMat.Get()).RetrieveResourse());
        world.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ 213,554,227 }, Math::vec3{ 130,0,0 }, Math::vec3{ 0,0,105 }, LightMat.Get()).RetrieveResourse());
        

        world.EmplaceBack(MakeUniqueHandle<Sphere>(Math::vec3{ 190,245,190 }, 37, Glass.Get()).RetrieveResourse());
        world.EmplaceBack(MakeUniqueHandle<Sphere>(Math::vec3{ 173,378,245 }, 17.5, Glass.Get()).RetrieveResourse());

        Math::vec3 SpheresSpawnOrigin{ 235,245,190 };
        const int32 MaxMetalSpheres = 100;
        for (int32 i = 0; i < MaxMetalSpheres; i++)
        {
        
            world.EmplaceBack(MakeUniqueHandle<Sphere>(SpheresSpawnOrigin + Util::RandomVector(-194.f, 194.f) + Math::vec3{ (float64)(i % 2),
                ((float64)(i % 10) / 10.0), ((float64)(i % 20) / 10) + 2}, 15.8, whiteMetal.Get()).RetrieveResourse());
        }
        
        const int32 MaxSolidSpheres = 50;
        for (int32 i = 0; i < MaxSolidSpheres; i++)
        {
            if (RTW::Util::randomDouble() > 0.75)
            {
                world.EmplaceBack(MakeUniqueHandle<Sphere>(SpheresSpawnOrigin + Util::RandomVector(-257.f, 189.f) + Math::vec3{ (float64)(i % 3),
                ((float64)(i % 10) / 5.0), ((float64)(i % 20) / 6) }, Util::randomDouble(10,35), Glass.Get()).RetrieveResourse());
            }
            else
            world.EmplaceBack(MakeUniqueHandle<Sphere>(SpheresSpawnOrigin + Util::RandomVector(-257.f,189.f) + Math::vec3{(float64)(i % 3),
                ((float64)(i % 10) / 5.0), ((float64)(i % 20) / 6)}, 15, whiteMat.Get()).RetrieveResourse());
        }

        Math::vec3 a = { 0,0,0};
        Math::vec3 b = {89,170,89};
        UniqueMemoryHandle<RayObject> box1 = ConstructBox(a, b, whiteMetal.Get()).RetrieveResourse();
        box1 = MakeUniqueHandle<RotateYOperation>(box1.RetrieveResourse(), 15).RetrieveResourse();
        box1 = MakeUniqueHandle<TranslatedObject>(box1.RetrieveResourse(), Math::vec3{ 265,0,295 }).RetrieveResourse();
        world.EmplaceBack(box1.RetrieveResourse());
        a = { 7,7,7};
        b = { 170,170,170 };
        UniqueMemoryHandle<RayObject> box2 = ConstructBox(a, b, whiteMat.Get()).RetrieveResourse();
        box2 = MakeUniqueHandle<RotateYOperation>(box2.RetrieveResourse(), 5).RetrieveResourse();
        box2 = MakeUniqueHandle<TranslatedObject>(box2.RetrieveResourse(), Math::vec3{ 120,0,150 }).RetrieveResourse();
        world.EmplaceBack(box2.RetrieveResourse());
        
        auto sphereW = MakeUniqueHandle<Sphere>(Math::vec3{400,235,325}, 95.5, Glass.Get());
        //auto volume = MakeUniqueHandle<ConstantMedium>(sphereW.RetrieveResourse(), 14, Math::color{ .835,.955,.995 });
        world.EmplaceBack(sphereW.RetrieveResourse());

        lights.EmplaceBack(MakeUniqueHandle<Quad>(Math::vec3{ 343,554,332 }, Math::vec3{ -130,0,0 }, Math::vec3{ 0,0,-105 }, emptyMat.Get()).RetrieveResourse());
        //world.EmplaceBack(MakeUniqueHandle<Sphere>(Math::vec3{ 190,90,190 }, 90, LightMat.Get()).RetrieveResourse());
        //lights.EmplaceBack(MakeUniqueHandle<Sphere>(Math::vec3{ 190,90,190 }, 90, emptyMat.Get()).RetrieveResourse());
        //sphereW = MakeUniqueHandle<Sphere>(Math::vec3{400,300,400 }, 150, whiteMat.Get());
        //auto volume = MakeUniqueHandle<ConstantMedium>(sphereW.RetrieveResourse(), 10, Math::color{ .78/10.0,.95/10.0,.99/10.0 });
        //world.EmplaceBack(volume.RetrieveResourse());

        Camera.SetViewPerspective({ 278,278,-800 }, { 278,278,0 }, {0,1,0});

        }
    break;
    default:
        break;
    }
};

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
    RTW::RayList::ObjectList ObjectList{ 50 };
    RTW::RayList::ObjectList Lights{ 50 };

    // Camera init
    RTW::RayCamera Camera(1000, 16.0/9.0);
    RTWGlobalState.FrameBufferWidth = Camera.GetImageWidth();
    RTWGlobalState.FrameBufferHeight = Camera.GetImageHeight();

    PopulateScene(ObjectList, Lights, Camera ,CornelBoxSmoke);
    //auto BVH = MakeSharedHandle<RTW::BVH_Node>(ObjectList, 0, ObjectList.size());
    //RTW::RayList World{ BVH.Get()};
    RTW::RayList World{ std::move(ObjectList)};
    RTW::RayList LightList{ std::move(Lights) };

    Camera.setPerPixelSamples(100);
    Camera.setDepth(50);
    Camera.setVFov(40);
    Camera.SetFocus(0.25, 12);
    Camera.initialize();
    Camera.backgroundColor = {.002,.002,.01};
    Camera.useBackgroundBlend = false;

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
    std::thread RenderThread([&]() { Camera.render(World, LightList); });
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
    // -----------------
    RTW::RayList::ObjectList ObjectList{ 50 };

    PopulateScene(ObjectList, SpheresParadise);

    auto BVH = MakeSharedHandle<RTW::BVH_Node>(ObjectList, 0, ObjectList.size());
    RTW::RayList World{ BVH.Get() };

    // Camera init
    RTW::RayCamera Camera(1000, 16.f / 9.f);
    RTWGlobalState.FrameBufferWidth = Camera.GetImageWidth();
    RTWGlobalState.FrameBufferHeight = Camera.GetImageHeight();
    Camera.setPerPixelSamples(500);
    Camera.setDepth(50);
    Camera.setVFov(20);
    Camera.SetViewPerspective({ 13,2,3 }, { 0.2,0.7,0.3 }, { 0,1,0 });
    Camera.SetFocus(0.6, 10);
    Camera.initialize();

    // START RTW RENDER
   // ----------------
    std::thread RenderThread([&]() {Camera.render(World); });
    // ----------------
    RenderThread.join();
    RTW_INFO("RayTracing application finished execution.");
    return 0;
}
#endif