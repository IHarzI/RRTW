// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing in One Weekend.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image(not multhithreaded).
//WIP.

#pragma once

#include <Ray.h>
#include "RTW_Memory.h"
#include "RTW_Material.h"

namespace RTW
{
	struct HitRecord {
		float32 t;
		Material* mat;
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