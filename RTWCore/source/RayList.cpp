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