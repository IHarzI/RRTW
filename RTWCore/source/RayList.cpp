// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image.
//WIP.
#include "RayList.h"

namespace RTW
{
	bool RayList::hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const
	{
		bool hitAnything = false;
		float64 closest = tMax;
		for(auto& Object : rayObjectsList)
		{
			HitRecord record;
			if (Object->hit(r, tMin, closest, record))
			{
				hitAnything = true;
				closest = record.t;
				rec = std::move(record);
			}
		}
		return hitAnything;
	}
};