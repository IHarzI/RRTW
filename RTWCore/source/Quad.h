// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once
#include "RTW_CORE.h"
#include "RayObject.h"
#include "RayList.h"
#include "AABB.h"

namespace RTW
{
	class Quad : public RayObject
	{
	public:
		Quad(const Math::vec3& Q, const Math::vec3& u, const Math::vec3& v, SharedMemoryHandle<Material> mat)
			: Q(Q), u(u), v(v), material(mat)
		{
			auto n = Math::CrossProduct(u, v);
			normal = n.GetNormalized();
			D = Math::DotProduct(normal, Q);
			w = n / Math::DotProduct(n, n);

			setBoundingBox();
		}

		virtual void setBoundingBox()
		{
			auto bBoxDiagonal1 = D3Math::AABB{ Q,Q + u + v };
			auto bBoxDiagonal2 = D3Math::AABB{ Q + u, Q + v};
			bBox = D3Math::AABB{ bBoxDiagonal1,bBoxDiagonal2 };
		}

		virtual D3Math::AABB boundingBox() const { return bBox; }

		bool hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const override;

		virtual bool isInterior(float64 a, float64 b, HitRecord& rec) const;

	private:
		Math::vec3 Q;
		Math::vec3 u;
		Math::vec3 v;
		Math::vec3 w;
		D3Math::AABB bBox;
		SharedMemoryHandle<Material> material;
		Math::vec3 normal;
		float64 D;
	};

	RTW_INLINE UniqueMemoryHandle<RayList> ConstructBox(const Math::vec3& a, const Math::vec3& b, SharedMemoryHandle<Material> mat)
	{
		auto sides = MakeUniqueHandle<RayList>();

		Math::vec3 min = { Math::min(a.x,b.x),Math::min(a.y,b.y),Math::min(a.z,b.z) };
		Math::vec3 max = { Math::max(a.x,b.x),Math::max(a.y,b.y),Math::max(a.z,b.z) };

		Math::vec3 dx = {max.x - min.x,0,0};
		Math::vec3 dy = { 0, max.y - min.y,0 };
		Math::vec3 dz = {0,0,max.z-min.z};

		auto top =MakeUniqueHandle<Quad>(Math::vec3{ min.x,max.y,max.z }, dx, -1 * dz, mat);
		auto bottom =MakeUniqueHandle<Quad>(Math::vec3{ min }, dx, dz, mat);
		auto left =MakeUniqueHandle<Quad>(Math::vec3{ min }, dz, dy, mat);
		auto right = MakeUniqueHandle<Quad>(Math::vec3{ max.x,min.y,max.z }, -1 * dz, dy, mat);
		auto back =MakeUniqueHandle<Quad>(Math::vec3{ max.x,min.y,min.z }, -1 * dx, dy, mat);
		auto front =MakeUniqueHandle<Quad>(Math::vec3{ min.x,min.y,max.z }, dx, dz, mat);

		sides->addObject(top.RetrieveResourse());
		sides->addObject(bottom.RetrieveResourse());					
		sides->addObject(left.RetrieveResourse());
		sides->addObject(right.RetrieveResourse());
		sides->addObject(back.RetrieveResourse());
		sides->addObject(front.RetrieveResourse());

		return sides;
	}

};