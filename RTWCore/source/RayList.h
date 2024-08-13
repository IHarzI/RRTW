// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include "RayObject.h"
#include "Containers/RTW_DynamicArray.h"
#include "AABB.h"

namespace RTW
{
	class RayList : public RayObject
	{
	public:
		using ObjectHandle = SharedMemoryHandle<RTW::RayObject>;
		using ObjectList = Containers::DynamicArray<ObjectHandle>;

		RayList() {};
		RayList(ObjectList list) : rayObjectsList(list){};
		RayList(ObjectHandle object) { addObject(object); };

		virtual bool hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const;
		void addObject(ObjectHandle object)
		{
			bBox = D3Math::AABB{ bBox, object->boundingBox() };
			rayObjectsList.EmplaceBack({ std::move(object) });
		}
		const ObjectList& GetObjectList() { return rayObjectsList; };

		D3Math::AABB boundingBox() const override { return bBox; }
	private:
		ObjectList rayObjectsList;
		D3Math::AABB bBox;
	};

};