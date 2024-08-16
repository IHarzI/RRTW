// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RayList.h"
#include "RTW_Context.h"

#include <iostream>
#include <fstream>
#include <random>

namespace RTW
{
	enum RayTraceResultFlags : uint64
	{
		OutOfDepth = 0,
		Background = 2,
		WorldHit = 4
	};
	struct RayTraceResult
	{
		Math::vec3 Color;
		uint64 Flags;
	};

	class RayCamera
	{
	public:
		RayCamera(int32 width, float32 aspectRatio, Math::vec3 center = {0,0,0});
		void initialize();
		void render(const RayObject& world, const RayObject& lights);
		void setPerPixelSamples(uint32 count) 
		{
			PerPixelSamples = count; 
			pixelSamplesScale = 1.0 / (float64)PerPixelSamples;
		}

		void setDepth(int32 Depth)
		{
			maxDepth = Depth;
		}

		void setVFov(float32 verticalFov)
		{
			VFov = verticalFov;
		}

		void SetViewPerspective(Math::vec3 LookFrom, Math::vec3 LookAt, Math::vec3 Up);
		void SetFocus(float32 Angle, float32 FocusDistance);

		int32 GetImageHeight() const { return imageHeight; };
		int32 GetImageWidth() const { return imageWidth; };
		Math::color backgroundColor;
		bool useBackgroundBlend = false;
	private:

		void RenderPixel(const RayObject& world, const RayObject& lights, Math::vec2i PixelCoords, int32 Depth, Containers::DynamicArray<char>* CharImageBuff = nullptr);
		Math::color RayColorTrace(const Ray& r, const RayObject& world, const RayObject& lights, int32 Depth);
		Ray getRay(int32 i, int32 j, int32 s_I, int32 s_J) const;

		Math::vec3 sampleSquareStratified(int32 s_I, int32 s_J) const;

		Math::vec3 sampleSquare() const;
		Math::vec3 defocusDiskSample() const;

 		float32 aspectRatio = 1.0;
		float64 pixelSamplesScale = 10.f;
		int32 imageWidth = 100;
		int32 imageHeight;
		int32 sqrtSpp;
		float64 recipSqrtSpp;
		uint32 PerPixelSamples = 10;
		int32 maxDepth = 10;
		float32 VFov = 90.f;

		float64 defocusAngle = .0;
		float64 focusDistance = 10;

		Math::vec3 center;
		Math::vec3 ulCorner;
		Math::vec3 pixelUOffset;
		Math::vec3 pixelVOffset;
		Math::vec3 lookFrom{};
		Math::vec3 lookAt{ .0f,.0f,-1.f };
		Math::vec3 upVector{ .0f,1.f,.0f };
		Math::vec3 u, v, w;
		Math::vec3 defocusDiskU;
		Math::vec3 defocusDiskV;
	};

};