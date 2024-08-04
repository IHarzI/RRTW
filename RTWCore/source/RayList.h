// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing in One Weekend.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image(not multhithreaded).
//WIP.

#pragma once

#include "RayObject.h"
#include "Containers/RTW_DynamicArray.h"
namespace RTW
{

	class RayList : public RayObject
	{
	public:
		using ObjectList = Containers::DynamicArray<UniqueMemoryHandle<RTW::RayObject>>;
		RayList() {};
		RayList(ObjectList list) : rayObjectsList(list){};
		virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const;
		ObjectList rayObjectsList;
	};

};