// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include <RayObject.h>
#include "AABB.h"
#include "RTW_Util.h"

namespace RTW
{
	class Sphere : public RayObject
	{
	public:
		Sphere() {};
		Sphere(Sphere& Other) { center = Other.center; radius = Other.radius; material = Other.material; isMoving = Other.isMoving; centerMoved = Other.centerMoved; };
		Sphere(Math::vec3 center, float32 r, SharedMemoryHandle<RTW_Material> material) : center(center), radius(r), material(material)
		{
			auto rvec = Math::vec3{ radius };
			bBox = D3Math::AABB{ center - rvec, center + rvec };
		};

		Sphere(Math::vec3 center, Math::vec3 center2, float32 r, SharedMemoryHandle<RTW_Material> material) : center(center), radius(r), material(material), centerMoved(center2), isMoving(true)
		{
			auto rvec = Math::vec3{ radius };
			auto bBox1 = D3Math::AABB{ center - rvec, center + rvec };
			auto bBox2 = D3Math::AABB{ center2 - rvec, center2 + rvec };
			bBox = D3Math::AABB{ bBox1, bBox2 };
		};
		virtual bool hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const;

		virtual D3Math::AABB boundingBox() const override {return bBox;};

		RTW_INLINE float64 pdf_value(const Math::vec3& origin, const Math::vec3& direction) const override
		{
			HitRecord rec;
			if (!this->hit(Ray(origin, direction), 0.001, Math::infinity<float64>(), rec))
				return 0;

			auto cosThetaMax = Math::sqrt(1 - radius * radius / (center - origin).SquareLength());
			auto solidAngle = 2 * Math::pi<float64>() * (1 - cosThetaMax);

			return 1 / solidAngle;
		}

		RTW_INLINE Math::vec3 random(const Math::vec3& origin) const override
		{
			Math::vec3 direction = center - origin;
			auto distanceSqr = direction.SquareLength();
			Util::ONB uvw{ direction };
			return uvw.transform(randomToSphere(radius, distanceSqr));
		}

	private:

		RTW_INLINE Math::vec3 CalculateCenterFromTime(float64 time) const
		{
			return center + time * centerMoved;
		};

		RTW_INLINE void getSphereUV(const Math::vec3& p, float64& u, float64& v) const
		{
			float64 theta = Math::acos(-p.y);
			float64 phi = Math::atan2(-p.z, p.x) + Math::pi();

			u = phi / (Math::two_pi());
			v = theta / Math::pi();
		};

		RTW_STATIC RTW_INLINE Math::vec3 randomToSphere(float64 radius, float64 disSqr)
		{
			auto r1 = Util::randomDouble();
			auto r2 = Util::randomDouble();
			auto z = 1 + r2 * (Math::sqrt(1 - radius * radius / disSqr) - 1);

			auto phi = 2 * Math::pi<float64>() * r1;
			auto x = Math::cos(phi) * Math::sqrt(1 - z * z);
			auto y = Math::sin(phi) * Math::sqrt(1 - z * z);

			return Math::vec3{ x,y,z };
		}

		Math::vec3 center;
		float32 radius;
		SharedMemoryHandle<RTW_Material> material;
		Math::vec3 centerMoved{};
		bool isMoving = false;
		D3Math::AABB bBox{};
	};

};

