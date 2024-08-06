// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RayList.h"

namespace RTW
{
	class BVH_Node : public RayObject
	{
	public:

		BVH_Node(const RayList::ObjectList& List, uint64 start, uint64 end);

		virtual bool hit(const Ray& r, float32 tMin, float32 tMax, HitRecord& rec) const;

		D3Math::AABB boundingBox() const override { return bBox; }

		RTW_STATIC bool boxCompare(
			const RayList::ObjectHandle a, const RayList::ObjectHandle b, int axis_index
		) {
			auto a_axis_interval = a->boundingBox().axisInterval(axis_index);
			auto b_axis_interval = b->boundingBox().axisInterval(axis_index);
			return a_axis_interval.min < b_axis_interval.min;
		}
	private:
		RayList::ObjectHandle left;
		RayList::ObjectHandle right;
		D3Math::AABB bBox;
	};
}