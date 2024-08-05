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
		Sphere(Sphere& Other) { center = Other.center; radius = Other.radius; material = Other.material; };
		Sphere(Math::vec3 center, float32 r, SharedMemoryHandle<Material> material) : center(center), radius(r), material(material) {};
		virtual bool hit(const Ray& r, float32 tMin, float32 tMax, HitRecord& rec) const;

		Math::vec3 center;
		float32 radius;
		SharedMemoryHandle<Material> material;
	};

};

