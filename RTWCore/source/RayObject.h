#pragma once

#include <Ray.h>

namespace RTW
{
	struct HitRecord {
		float32 t;
		Math::vec3 p;
		Math::vec3 normal;
		uint8 frontFace : 1;

		void setFaceNormal(const Ray& r, const Math::vec3 outwardNormal);
	};

	class RayObject
	{
	public:
		virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const = 0;
	};

};