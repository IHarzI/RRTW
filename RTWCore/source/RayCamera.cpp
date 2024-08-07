// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "RayCamera.h"
#include "RTW_Util.h"
#include "RTW_Logger.h"
#include "RTW_Context.h"
#include "RTW_Memory.h"
#include "RTW_Material.h"
#include "Containers/RTW_DynamicArray.h"

#include <thread>
#include <future>

#define WRITE_TO_IMAGE 1

#define RENDER_ON_SURFACE 1

#define RENDER_MULTITHREAD 1
const uint32 maxThreads = 13;

namespace RTW
{
	RayCamera::RayCamera(int32 width, float32 aspectRatio, Math::vec3 center)
	{
		imageWidth = width;
		this->aspectRatio = aspectRatio;
		this->center = center;
		//initialize();
	}

	void RayCamera::render(const RayList& world)
	{
		Containers::DynamicArray<char>* CharImageBuffPtr = nullptr;
#if WRITE_TO_IMAGE == 1
		Containers::DynamicArray<char> CharImageBuff{};
		CharImageBuff.Resize(imageWidth * imageHeight * RTW::Util::PixelImageFormatSizes[RTW::Util::P3M]);
		CharImageBuffPtr = &CharImageBuff;
		std::ofstream Image;
		Image.open("assets/exampleImage.ppm");
		Image << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
#endif

#if RENDER_MULTITHREAD == 1
		int32 RenderThreadPoolIndex = 0;
		Containers::DynamicArray<std::future<void>> ThreadPool{maxThreads};

		for (int32 iThread = 0; iThread < maxThreads; iThread++)
		{
			const int32 ImageThreadPart = (int32)((float64)imageHeight / (float64)maxThreads);;
			int32 YStartThreadOffset = ImageThreadPart * (iThread);
			int32 YEndThreadOffset = YStartThreadOffset + ImageThreadPart;
			YStartThreadOffset = Math::clamp(YStartThreadOffset,0, imageHeight - ImageThreadPart);
			YEndThreadOffset = Math::clamp(YEndThreadOffset, YStartThreadOffset, imageHeight);

			auto RenderF = [&world, CharImageBuffPtr, this](int32 YStartThreadOffset, int32 YEndThreadOffset, 
				Containers::DynamicArray<char>* ImageCharBuff)
			{

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
						RenderPixel(world, { XPixelIndex,YPixelIndex }, maxDepth, CharImageBuffPtr);
					}
				}
			};

			auto result = std::async(std::launch::async, RenderF, YStartThreadOffset, YEndThreadOffset, CharImageBuffPtr);
			ThreadPool.EmplaceBack(std::move(result));
			
			RenderThreadPoolIndex++;
		};

		for (auto& thread : ThreadPool)
		{
			thread.get();
		}
		RenderThreadPoolIndex = 0;
		ThreadPool.Clear();
#else
		for (int32 Y = 0; Y < imageHeight; Y++)
		{

			if (!GetRTWGlobalState().IsRunning)
			{
				RTW_INFO("Rendering canceled");
				return;
			}

			RTW_INFO("Progress remained: %i", imageHeight - Y);
			for (int32 X = 0; X < imageWidth; X++)
			{

				RenderPixel( world, { X,Y }, maxDepth, CharImageBuffPtr);
			}
		}
#endif

#if WRITE_TO_IMAGE == 1
		Image.write(CharImageBuff.data(), CharImageBuff.size());
		Image.close();
#endif

		RTW_INFO("DONE");
	}

	void RayCamera::SetViewPerspective(Math::vec3 LookFrom, Math::vec3 LookAt, Math::vec3 Up)
	{
		lookFrom = LookFrom;
		lookAt = LookAt;
		upVector = Up;
	}

	void RayCamera::SetFocus(float32 Angle, float32 FocusDistance)
	{
		defocusAngle = Angle;
		focusDistance = FocusDistance;
	}

	void RayCamera::initialize()
	{
		center = lookFrom;
		imageHeight = imageWidth / aspectRatio;
		imageHeight = imageHeight < 1 ? 1 : imageHeight;

		float32 theta = Math::radians(VFov);
		float32 h = Math::tan(theta / 2);
		float32 viewportHeight = 2.f * h * focusDistance;
		float32 viewportWidth = viewportHeight * (float32(imageWidth) / imageHeight);

		w = (lookFrom - lookAt).Normalize();
		u = upVector.CrossProduct(w);
		v = w.CrossProduct(u);

		Math::vec3 viewportU = viewportWidth * u;
		Math::vec3 viewportV = viewportHeight * v * -1.f;

		pixelUOffset = viewportU / imageWidth;
		pixelVOffset = viewportV / imageHeight;

		Math::vec3 viewportUL = center - (w * focusDistance) - viewportU / 2 - viewportV / 2;
		ulCorner = viewportUL + .5f * (pixelUOffset + pixelVOffset);

		float64 defocusRadius = focusDistance * Math::tan(Math::radians(defocusAngle / 2.f));
		defocusDiskU = u * defocusRadius;
		defocusDiskV = v * defocusRadius;
	}

	void RayCamera::RenderPixel(const RayList& world, Math::vec2i PixelCoords, int32 Depth, Containers::DynamicArray<char>* CharImageBuff)
	{
		Math::color pixelColor{ 0, 0, 0 };

		for (int32 sample = 0; sample < PerPixelSamples; sample++)
		{
			Ray r = getRay(PixelCoords.x, PixelCoords.y);
			pixelColor += RayColorTrace(r, world, maxDepth) * pixelSamplesScale;
		}

#if RENDER_MULTITHREAD == 1
		//std::lock_guard<std::mutex> WriteMutexGuard{ GlobalContext.MultithreadPixelWriteMutex };
#endif

#if WRITE_TO_IMAGE == 1
		if (CharImageBuff)
		{
			Util::writeColor(CharImageBuff->data(), pixelColor, {imageWidth,imageHeight}, PixelCoords);
		};
#endif

#if RENDER_ON_SURFACE == 1
		uint32* CurrentPixel = GetRTWGlobalState().FrameBufferPixels + (PixelCoords.y * imageWidth) + PixelCoords.x;
		Util::writePixelColor(CurrentPixel, pixelColor);
#endif
	}

	Math::color RayCamera::RayColorTrace(const Ray& r, const RayList& world, int32 Depth)
	{
		if (Depth <= 0)
		{
			return { 0, 0, 0 };
		}
		else
		{
			RTW::HitRecord hitRecord{};
			if (world.hit(r, 0.001f, Math::MaxFloat64(), hitRecord))
			{

				Ray Scattered{};
				Math::color Attenuation{};
				if (hitRecord.mat->scatter(&r, &hitRecord, Attenuation, &Scattered))
				{
					return Attenuation * RayColorTrace(Scattered, world, Depth - 1);
				}
				else
				{
					return Math::color{ 0 };
				};
			}
			else
			{
				RTW::Math::vec3 unitDirection = RTW::Math::Normalize(r.direciton());
				float64 t = 0.5f * (unitDirection.y + .5f);
				return (Math::color)RTW::Math::Lerp({ 1,1,1 }, { .2,.4,1 }, t);
			};
		};
		return { 0.f };
	}

	Math::vec3 RayCamera::sampleSquare() const {
		// Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
		return { (float32)(Util::randomDouble() - 0.5), (float32)(Util::randomDouble() - 0.5), .0f };
	}

	Math::vec3 RayCamera::defocusDiskSample() const
	{
		Math::vec3 p = Util::RandomUnitDiskVector();
		return center + (p[0] * defocusDiskU) + (p[1] * defocusDiskV);
	}

	Ray RayCamera::getRay(int32 i, int32 j) const
	{
		Math::vec3 offset = sampleSquare();
		Math::vec3 pixelSample = ulCorner + ((i + offset.x) * pixelUOffset) + ((j + offset.y) * pixelVOffset);
		Math::vec3 rayOrigin = (Math::less_or_equal(defocusAngle, 0) ? center : defocusDiskSample());
		Math::vec3 rayDirection = pixelSample - rayOrigin;
		float64 rayTime = Util::randomDouble(0.f, 1.f);
		return Ray(rayOrigin, rayDirection, rayTime);
	}
};
