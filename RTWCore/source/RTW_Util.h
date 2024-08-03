#pragma once

#include "RTW_CORE.h"
#include "RTW_MATH.h"
#include <random>
#include <fstream>

namespace RTW
{
	namespace Util
	{
		void writeColor(std::ofstream& stream, const RTW::Math::vec3 color);

		void writePixelColor(uint32* Pixel, Math::vec3 color);

		RTW_INLINE float64 linearToGamma(float64 linearComponent)
		{
			if (Math::more(linearComponent, 0.f))
			{
				return Math::sqrt(linearComponent);
			}
			return .0;
		}

		// Returns float64 in range -1.f to 1.f
		RTW_INLINE float64 randomDoubleGenerate() {
			RTW_STATIC std::uniform_real_distribution<float64> distribution(-1.f, 1.f);
			RTW_STATIC std::mt19937 generator;
			return distribution(generator);
		}

		// Return random duoble in clamped range. Note, that raw random value is in range -1.f to 1.f, therefore, min/max more that abs(1.f) is meanless.
		// For getting bigger flaot, just multiply by int
		RTW_INLINE float64 randomDouble(float64 min = 0.f, float64 max = 1.f) {
			return Math::clamp(randomDoubleGenerate(), min, max);
		}

		RTW_INLINE Math::vec3 RandomVector(float32 min, float32 max)
		{
			Math::vec3 Result = {
				(float32)randomDouble(min,max),
				(float32)randomDouble(min,max),
				(float32)randomDouble(min,max)
			};
			return Result;
		}

		RTW_INLINE Math::vec3 RandomUnitSphereVector()
		{
			//Devil loop
			while (true)
			{
				Math::vec3 vector = RandomVector(-1.f, 1.f);
				if (Math::less(vector.SquareLength(), 1.f))
				{
					return vector;
				}
			}
		};

		RTW_INLINE Math::vec3 RandomUnitSphereUnitVector()
		{
			return RandomUnitSphereVector().Normalize();
		};

		RTW_INLINE Math::vec3 RandomHemisphereUnitVector(const Math::vec3 normal)
		{
			Math::vec3 UnitSphereVec = RandomUnitSphereUnitVector();
			if (Math::more(Math::DotProduct(UnitSphereVec, normal), 0.f))
			{
				return UnitSphereVec;
			}
			return UnitSphereVec * -1.f;
		};
	}

};