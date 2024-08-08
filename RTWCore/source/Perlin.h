// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RTW_Util.h"

namespace RTW
{
	class Perlin
	{
	public:
		Perlin() {
			for (int32 i = 0; i < pointCount; i++)
			{
				randVec[i] = Util::RandomVector(-1,1).Normalize();
			}

			perlinGeneratePerm(permX);
			perlinGeneratePerm(permY);
			perlinGeneratePerm(permZ);
		}

		float64 noise(const Math::vec3& p) const
		{

			auto u = p.x - Math::floor(p.x);
			auto v = p.y - Math::floor(p.y);
			auto w = p.z - Math::floor(p.z);

			auto i = int32(Math::floor(p.x));
			auto j = int32(Math::floor(p.y));
			auto k = int32(Math::floor(p.z));

			Math::vec3 c[2][2][2];

			for (int32 di = 0; di < 2; di++)
			{
				for (int32 dj = 0; dj < 2; dj++)
				{
					for (int32 dk = 0; dk < 2; dk++)
					{
						c[di][dj][dk] = randVec[
							permX[(i + di) & 255] ^
								permY[(j + dj) & 255] ^
								permZ[(k + dk) & 255]
						];
					}
				}
			}
			return PerlinInterp(c, u, v, w);
		}
	private:

		RTW_STATIC RTW_INLINE void perlinGeneratePerm(int32* p)
		{
			for (int32 i = 0; i < pointCount; i++)
			{
				p[i] = i;
			}
			permute(p, pointCount);
		}

		RTW_STATIC RTW_INLINE void permute(int32* p, int32 n)
		{
			for (int32 i = n - 1; i > 0; i--)
			{
				int32 target = Util::randomInt(0, i);
				int32 tmp = p[i];
				p[i] = p[target];
				p[target] = tmp;
			}
		}

		RTW_STATIC RTW_INLINE float64 PerlinInterp(Math::vec3 c[2][2][2], float64 u, float64 v, float64 w)
		{
			float64 uu = u * u * (3 - 2 * u);
			float64 vv = v * v * (3 - 2 * v);
			float64 ww = w * w * (3 - 2 * w);
			float64 accum = 0.0;

			for (int32 i = 0; i < 2; i++)
			{
				for (int32 j = 0; j < 2; j++)
				{
					for (int32 k = 0; k < 2; k++)
					{
						Math::vec3 weightV(u - i, v - j, w - k);
						accum += (i * uu + (1 - i) * (1 - uu)) *
							(j * vv + (1 - j) * (1 - vv)) *
							(k * ww + (1 - k) * (1 - ww)) *
							c[i][j][k].DotProduct(weightV);
					}
				}
			}
			return accum;
		}

		RTW_STATIC const int32 pointCount = 256;
		Math::vec3 randVec[pointCount];
		int32 permX[pointCount];
		int32 permY[pointCount];
		int32 permZ[pointCount];
	};

};

