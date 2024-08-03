// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing in One Weekend.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image(not multhithreaded).
//WIP.

#pragma once

#include <RayObject.h>

namespace RTW
{
	class Sphere : public RayObject
	{
	public:
		Sphere() {};
		Sphere(Math::vec3 center, float32 r) : center(center), radius(r) {};
		virtual bool hit(const Ray& r, float32 tMin, float32 tMax, HitRecord& rec) const;

		Math::vec3 center;
		float32 radius;
	};

};

