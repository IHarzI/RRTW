#pragma once

#include "RTW_CORE.h"
#include "RayList.h"
#include "RTW_Context.h"

#include <iostream>
#include <fstream>
#include <random>

namespace RTW
{
	class RayCamera
	{
	public:
		RayCamera(int32 width, float32 aspectRatio, Math::vec3 center = {0,0,0});
		void render(const RayList& world);
		void setPerPixelSamples(uint32 count) 
		{
			PerPixelSamples = count; 
			pixelSamplesScale = 1.0 / PerPixelSamples;
		}

		void setDepth(int32 Depth)
		{
			maxDepth = Depth;
		}

		int32 GetImageHeight() const { return imageHeight; };
		int32 GetImageWidth() const { return imageWidth; };
	private:

		void initialize();
		void RenderPixel(const RayList& world, Math::vec2i PixelCoords, int32 Depth, std::ofstream* ImageRenderResult = nullptr);
		Math::vec3 RayColorTrace(const Ray& r, const RayList& world, int32 Depth);
		Ray getRay(int32 i, int32 j) const;
		Math::vec3 sampleSquare() const;
 		float32 aspectRatio = 1.0;
		float64 pixelSamplesScale;
		int32    imageWidth = 100;
		int32 imageHeight;
		uint32 PerPixelSamples = 10;
		int32 maxDepth = 10;
		Math::vec3 center;
		Math::vec3 llCorner;
		Math::vec3 pixelUOffset;
		Math::vec3 pixelVOffset;
	};

};