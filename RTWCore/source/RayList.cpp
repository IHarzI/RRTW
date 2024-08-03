// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing in One Weekend.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image(not multhithreaded).
//WIP.

#include "RayList.h"

namespace RTW
{
	bool RayList::hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const
	{
		HitRecord record;
		bool hitAnything = false;
		float64 closest = tMax;
		for (int32 i = 0; i < size; i++)
		{
			if (List[i]->hit(r, tMin, closest, record))
			{
				hitAnything = true;
				closest = record.t;
				rec = record;
			}
		}
		return hitAnything;
	}
};