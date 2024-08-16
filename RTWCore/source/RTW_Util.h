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

		void MakeValidColor(Math::color& inColor);

		void writeColor(char* ImageCharBuff, Math::color color, const Math::vec2i imageSize, const Math::vec2i PixelCoords);

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

		RTW_STATIC RTW_INLINE Math::vec3 RandomCosineDirection()
		{
			auto r1 = randomDouble();
			auto r2 = randomDouble();

			auto phi = 2 * Math::pi<float64>()* r1;
			auto x = Math::cos(phi) * Math::sqrt(r2);
			auto y = Math::sin(phi) * Math::sqrt(r2);
			auto z = Math::sqrt(1 - r2);

			return { x,y,z };
		};

		struct ONB
		{
			ONB(const Math::vec3& n)
			{
				axisMatrix.zVector = n.GetNormalized();
				Math::vec3 a = Math::abs(axisMatrix.zVector.x) > 0.9 ? Math::vec3{0, 1, 0} : Math::vec3{1, 0, 0};
				axisMatrix.yVector = Math::CrossProduct(axisMatrix.zVector, a).Normalize();
				axisMatrix.xVector = Math::CrossProduct(axisMatrix.zVector, axisMatrix.yVector);
			}

			const Math::vec3& u() const { return axisMatrix.xVector; }
			const Math::vec3& v() const { return axisMatrix.yVector; }
			const Math::vec3& w() const { return axisMatrix.zVector; }

			Math::vec3 transform(const Math::vec3& v) const
			{
				return (v.x * axisMatrix.xVector) + (v.y* axisMatrix.yVector) + (v.z * axisMatrix.zVector);
			}
			Math::mat3x3 axisMatrix;
		};
	}

};