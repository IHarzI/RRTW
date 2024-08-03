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