// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "RayList.h"

namespace RTW
{

	bool RayList::hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const
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
				rec = record;
			}
		}
		return hitAnything;
	}

	float64 RayList::pdf_value(const Math::vec3& origin, const Math::vec3& direction) const
	{
		auto weight = 1.0 / rayObjectsList.size();
		auto sum = 0.0;

		for (auto& object : rayObjectsList)
		{
			sum += weight * object->pdf_value(origin, direction);
		}
		return sum;
	}
};