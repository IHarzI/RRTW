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

