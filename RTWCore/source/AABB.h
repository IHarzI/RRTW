// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RTW_MATH.h"
#include "RTW_Logger.h"
#include "Ray.h"

namespace RTW
{
	namespace D3Math
	{
		struct Interval {
			Interval() : min(RTW::Math::infinity<float64>()), max(-RTW::Math::infinity<float64>()) {};
			Interval(float64 min, float64 max) : min(min), max(max) {};
			Interval(const Interval& a, const Interval& b)
			{
				min = Math::less_or_equal(a.min, b.min) ? a.min : b.min;
				max = Math::more_or_equal(a.max, b.max) ? a.max : b.max;
			}
			float64 min, max;

			RTW_INLINE float64 size() const {
				return max - min;
			}

			RTW_INLINE bool contains(float64 x) const {
				return min <= x && x <= max;
			}

			RTW_INLINE bool surrounds(float64 x) const {
				return min < x && x < max;
			}

			RTW_INLINE Interval expand(float64 delta) const
			{
				float64 padding = delta / 2.0;
				return Interval{min-padding, max+padding};
			}

		};
		RTW_STATIC RTW_INLINE const Interval empty{ RTW::Math::infinity<float64>(), -RTW::Math::infinity<float64>() };
		RTW_STATIC RTW_INLINE const Interval universe{ -RTW::Math::infinity<float64>(), RTW::Math::infinity<float64>() };

		struct AABB {
			union
			{
				Interval x, y, z;
				Interval intervals[3];
			};
			AABB() {}

			AABB(const Interval& x, const Interval& y, const Interval& z)
				: x(x), y(y), z(z) {}

			AABB(const Math::vec3& a, const Math::vec3& b) {
				// Treat the two points a and b as extrema for the bounding box, so we don't require a
				// particular minimum/maximum coordinate order.

				x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
				y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
				z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
			}

			AABB(const AABB& bBox1, const AABB& bBox2)
			{
				x = { bBox1.x, bBox2.x };
				y = { bBox1.y, bBox2.y };
				z = { bBox1.z, bBox2.z };
			}

			const Interval& axisInterval(uint32 n) const
			{
				RTW_ASSERT(n < 3);
				return intervals[n];
			}

			bool overlap(const Ray& r, Interval rayT) const
			{
				const Math::vec3 rayOrigin = r.origin();
				const Math::vec3 rayDirection = r.direciton();

				for (uint32 axisIndex = 0; axisIndex < 3; axisIndex++)
				{
					const Interval& axis = intervals[axisIndex];
					const float64 adinv = 1.0 / rayDirection[axisIndex];

					auto t0 = (axis.min - rayOrigin[axisIndex]) * adinv;
					auto t1 = (axis.max - rayOrigin[axisIndex]) * adinv;

					if (t0 < t1)
					{
						if (t0 > rayT.min) rayT.min = t0;
						if (t1 < rayT.max) rayT.max = t1;
					}
					else
					{
						if (t1 > rayT.min) rayT.min = t1;
						if (t0 < rayT.max) rayT.max = t0;
					}

					if (Math::less_or_equal(rayT.max, rayT.min))
					{
						return false;
					}
					return true;
				}
			}
		};
	};
}