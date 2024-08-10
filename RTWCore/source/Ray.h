// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RTW_MATH.h"

namespace RTW
{
	class Ray
	{
	public:
		Ray() {};
		Ray(Math::vec3 A, Math::vec3 B, float64 Time) : a(A), b(B), tm(Time) {}
		Ray(Math::vec3 A, Math::vec3 B) : a(A), b(B) {}
		Math::vec3 origin() const { return a; }
		Math::vec3 direction() const { return b; }
		Math::vec3 pointAtScalar(float32 t) const { return a + t * b; };

		Math::vec3 a;
		Math::vec3 b;
		float64 tm;
	};
};