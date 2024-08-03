#include "RayCamera.h"
#include "RTW_Util.h"

#include "RTW_Logger.h"

#include "RTW_Context.h"
#include "Containers/RTW_DynamicArray.h"
#include "RTW_Memory.h"

#include <thread>
#include <mutex>
#define WRITE_TO_IMAGE 0

#define RENDER_ON_SURFACE 1

#define RENDER_MULTITHREAD 1
const uint32 maxThreads = 6;

namespace RTW
{
	RayCamera::RayCamera(int32 width, float32 aspectRatio, Math::vec3 center)
	{
		imageWidth = width;
		this->aspectRatio = aspectRatio;
		this->center = center;
		initialize();
	}

	void RayCamera::render(const RayList& world)
	{
		std::ofstream* ImageResoultTarget = nullptr;
#if WRITE_TO_IMAGE == 1
		std::ofstream Image;
		Image.open("assets/exampleImage.ppm");
		Image << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
		ImageResoultTarget = &Image;
#endif

#if RENDER_MULTITHREAD == 1
		int32 RenderThreadPoolIndex = 0;
		Containers::DynamicArray<UniqueMemoryHandle<std::thread>> ThreadPool{maxThreads};

		for (int32 iThread = 0; iThread < maxThreads; iThread++)
		{
			const int32 ImageThreadPart = (int32)((float64)imageHeight / (float64)maxThreads);;
			int32 YStartThreadOffset = ImageThreadPart * (iThread);
			int32 YEndThreadOffset = YStartThreadOffset + ImageThreadPart;
			YStartThreadOffset = Math::clamp(YStartThreadOffset,0, imageHeight - ImageThreadPart);
			YEndThreadOffset = Math::clamp(YEndThreadOffset, YStartThreadOffset, imageHeight);

			auto RenderF = [&world, this](int32 YStartThreadOffset, int32 YEndThreadOffset,	std::ofstream* ImageResoultTarget) {
				for (int32 YPixelIndex = YStartThreadOffset; YPixelIndex < YEndThreadOffset; YPixelIndex++)
				{
					for (int32 XPixelIndex = 0; XPixelIndex < imageWidth; XPixelIndex++)
					{
						if (!RTW::GetRTWGlobalState().IsRunning)
						{
							RTW_INFO("Rendering canceled, threadID: %i", std::this_thread::get_id());
							return;
						}
						//RTW_INFO("THREAD ID : %i  COORDS{X: %i, Y: %i}\n\0", std::this_thread::get_id(), XPixelIndex, YPixelIndex);
						RenderPixel(world, { XPixelIndex,YPixelIndex }, maxDepth, ImageResoultTarget);
					}
				}
			};

			ThreadPool.EmplaceBack(rtw_new<std::thread>(RenderF, YStartThreadOffset, YEndThreadOffset, ImageResoultTarget));
			
			RenderThreadPoolIndex++;
		};

		for (auto& thread : ThreadPool)
		{
			thread->join();
			thread.Release();
		}
		RenderThreadPoolIndex = 0;
		ThreadPool.Clear();
#else
		for (int32 Y = 0; Y < imageHeight; Y++)
		{

			if (!GlobalContext.IsRunning)
			{
				RTW_INFO("Rendering canceled");
				return;
			}

			RTW_INFO("Progress remained: %i", imageHeight - Y);
			for (int32 X = 0; X < imageWidth; X++)
			{

				RenderPixel(GlobalContext, world, { X,Y }, maxDepth, ImageResoultTarget);
			}
		}
#endif

#if WRITE_TO_IMAGE == 1
		Image.close();
#endif

		RTW_INFO("DONE");
	}

	void RayCamera::initialize()
	{
		imageHeight = imageWidth / aspectRatio;
		imageHeight = imageHeight < 1 ? 1 : imageHeight;
		setPerPixelSamples(10);

		float32 focalLenght = 1.f;
		float32 viewportHeight = 2.f;
		float32 viewportWidth = viewportHeight * (float32(imageWidth) / imageHeight);

		Math::vec3 viewportU = Math::vec3(viewportWidth, .0f, .0f);
		Math::vec3 viewportV = Math::vec3(.0f, -viewportHeight, .0f);

		pixelUOffset = viewportU / imageWidth;
		pixelVOffset = viewportV / imageHeight;

		Math::vec3 viewportUL = center - Math::vec3{.0f,.0f,focalLenght} - viewportU/2 - viewportV/2;
		llCorner = viewportUL + .5f * (pixelUOffset + pixelVOffset);
	}

	void RayCamera::RenderPixel(const RayList& world, Math::vec2i PixelCoords, int32 Depth, std::ofstream* ImageRenderResult)
	{
		Math::vec3 pixelColor{ 0.f, 0.f, 0.f };
		for (int32 sample = 0; sample < PerPixelSamples; sample++)
		{
			Ray r = getRay(PixelCoords.x, PixelCoords.y);
			pixelColor += RayColorTrace(r, world, maxDepth);
		}

		pixelColor = (float32)pixelSamplesScale * pixelColor;

#if RENDER_MULTITHREAD == 1
		//std::lock_guard<std::mutex> WriteMutexGuard{ GlobalContext.MultithreadPixelWriteMutex };
#endif

#if WRITE_TO_IMAGE == 1
		if (ImageResoultTarget)
		{
			Util::writeColor(*ImageRenderResult, pixelColor);
		};
#endif

#if RENDER_ON_SURFACE == 1
		uint32* CurrentPixel = GetRTWGlobalState().FrameBufferPixels + (PixelCoords.y * imageWidth) + PixelCoords.x;
		Util::writePixelColor(CurrentPixel, pixelColor);
#endif
	}

	Math::vec3 RayCamera::RayColorTrace(const Ray& r, const RayList& world, int32 Depth)
	{
		if (Depth <= 0)
		{
			return { 0.f,0.f,0.f };
		};

		RTW::HitRecord hitRecord{};
		if (world.hit(r, 0.001f, Math::MaxFloat64(), hitRecord))
		{
			Math::vec3 Dir = hitRecord.normal + Util::RandomHemisphereUnitVector(hitRecord.normal);
			Ray NewRay(hitRecord.p, Dir);
			const float32 reflectance = 0.575f;
			return reflectance * RayColorTrace(NewRay,world, Depth-1);
		}
		else
		{
			RTW::Math::vec3 unitDirection = RTW::Math::Normalize(r.direciton());
			float32 t = 0.5f * (unitDirection.y + .5f);
			return RTW::Math::Lerp({ 1.f,1.f,1.f }, { .2f,.4f,1.f }, t);
		};
	}

	Math::vec3 RayCamera::sampleSquare() const {
		// Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
		return { (float32)(Util::randomDouble() - 0.5), (float32)(Util::randomDouble() - 0.5), .0f };
	}

	Ray RayCamera::getRay(int32 i, int32 j) const
	{
		Math::vec3 offset = sampleSquare();
		Math::vec3 pixelSample = llCorner + ((i + offset.x) * pixelUOffset) + ((j + offset.y) * pixelVOffset);
		Math::vec3 rayOrigin = center;
		Math::vec3 rayDirection = pixelSample - rayOrigin;

		return Ray(rayOrigin, rayDirection);
	}
};
