// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RTW_MATH.h"
#include <random>
#include <fstream>
#include <thread>

namespace RTW
{
	namespace Util
	{
		enum PixelImageFormat : uint32
		{
			P3M = 0
		};

		RTW_STATIC constexpr uint32 PixelImageFormatSizes[] =
		{
			12
		};

		RTW_STATIC constexpr const char* PixelImageCharFormat[] = {
			"%03d %03d %03d\n"
		};

		void writeColor(char* ImageCharBuff, const Math::color color, const Math::vec2i imageSize, const Math::vec2i PixelCoords);

		void writePixelColor(uint32* Pixel, Math::color color);

		RTW_STATIC RTW_INLINE float64 linearToGamma(float64 linearComponent)
		{
			if (Math::more(linearComponent, 0))
			{
				return Math::sqrt(linearComponent);
			}
			return .0;
		}

		// Returns float64 in range .0f to 1.f
		RTW_STATIC RTW_INLINE float64 randomDoubleGenerate() {
			thread_local RTW_STATIC std::uniform_real_distribution<float64> distribution(0.f, 1.f);
			thread_local RTW_STATIC std::mt19937 generator(std::hash<std::thread::id>()(std::this_thread::get_id()));
			return distribution(generator);
		}

		RTW_STATIC RTW_INLINE float64 randomDouble(float64 min = 0.f, float64 max = 1.f) {
			return min + (max - min) * randomDoubleGenerate();
		}

		RTW_STATIC RTW_INLINE int32 randomInt(float64 min = 0.f, float64 max = 1.f) {
			return (int32)randomDouble(min, max + 1);
		}

		RTW_STATIC RTW_INLINE Math::vec3 RandomVector(float64 min, float64 max)
		{
			Math::vec3 Result = {
				randomDouble(min,max),
				randomDouble(min,max),
				randomDouble(min,max)
			};
			return Result;
		}

		RTW_STATIC RTW_INLINE Math::color RandomColor(float64 min, float64 max)
		{
			return (Math::color)RandomVector(min, max);
		}

		RTW_STATIC RTW_INLINE Math::vec3 RandomUnitSphereVector()
		{
			//Devil loop
			while (true)
			{
				Math::vec3 vector = RandomVector(-1, 1);
				if (Math::less(vector.SquareLength(), 1))
				{
					return vector;
				}
			}
		};

		RTW_STATIC RTW_INLINE Math::vec3 RandomUnitDiskVector()
		{
			//Devil loop
			while (true)
			{
				Math::vec3 vector{ (float32)randomDouble(-1,1), (float32)randomDouble(-1,1), .0};
				if (Math::less(vector.SquareLength(), 1))
				{
					return vector;
				}
			}
		};

		RTW_STATIC RTW_INLINE Math::vec3 RandomUnitSphereUnitVector()
		{
			return RandomUnitSphereVector().Normalize();
		};

		RTW_STATIC RTW_INLINE Math::vec3 RandomHemisphereUnitVector(const Math::vec3 normal)
		{
			Math::vec3 UnitSphereVec = RandomUnitSphereUnitVector();
			if (Math::more(Math::DotProduct(UnitSphereVec, normal), 0))
			{
				return UnitSphereVec;
			}
			return UnitSphereVec * -1;
		};
	}

};