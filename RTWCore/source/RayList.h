// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing in One Weekend.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image(not multhithreaded).
//WIP.

#pragma once

#include "RayObject.h"

namespace RTW
{

	class RayList : public RayObject
	{
	public:
		RayList() {};
		RayList(RayObject** list, int32 count) : List(list), size(count) {};
		virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const;
		RayObject** List;
		int32 size;
	};

};