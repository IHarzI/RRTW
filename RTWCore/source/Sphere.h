// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include <RayObject.h>
#include "AABB.h"

namespace RTW
{
	class Sphere : public RayObject
	{
	public:
		Sphere() {};
		Sphere(Sphere& Other) { center = Other.center; radius = Other.radius; material = Other.material; isMoving = Other.isMoving; centerMoved = Other.centerMoved; };
		Sphere(Math::vec3 center, float32 r, SharedMemoryHandle<Material> material) : center(center), radius(r), material(material) 
		{
			auto rvec = Math::vec3{ radius };
			bBox = D3Math::AABB{ center - rvec, center + rvec };
		};

		Sphere(Math::vec3 center, Math::vec3 center2, float32 r, SharedMemoryHandle<Material> material) : center(center), radius(r), material(material), centerMoved(center2), isMoving(true)
		{
			auto rvec = Math::vec3{ radius };
			auto bBox1 = D3Math::AABB{ center - rvec, center + rvec };
			auto bBox2 = D3Math::AABB{ center2 - rvec, center2 + rvec };
			bBox = D3Math::AABB{ bBox1, bBox2 };
		};
		virtual bool hit(const Ray& r, float32 tMin, float32 tMax, HitRecord& rec) const;

		virtual D3Math::AABB boundingBox() const {return bBox;};
		private:

		RTW_INLINE Math::vec3 CalculateCenterFromTime(float64 time) const
		{
			return center + time * centerMoved;
		};

		Math::vec3 center;
		float32 radius;
		SharedMemoryHandle<Material> material;
		Math::vec3 centerMoved{};
		bool isMoving = false;
		D3Math::AABB bBox{};
	};

};

