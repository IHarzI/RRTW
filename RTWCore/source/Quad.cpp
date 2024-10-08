// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "Quad.h"

namespace RTW
{

	float64 Quad::pdf_value(const Math::vec3& origin, const Math::vec3& direction) const
	{
		HitRecord rec;
		if (!this->hit(Ray(origin, direction), 0.0001, Math::infinity<float64>(), rec))
			return 0;


		auto distSqr = rec.t * rec.t * direction.SquareLength();
		auto cosine = Math::abs(Math::DotProduct(direction, rec.normal) / direction.Lenght());

		return distSqr / (cosine * area);
	}

	Math::vec3 Quad::random(const Math::vec3& origin) const
	{
		auto p = Q + (Util::randomDouble() * u) + (Util::randomDouble() * v);
		return p - origin;
	}

	bool Quad::hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const
	{
		auto denom = Math::DotProduct(normal, r.direction());
		if (Math::less(Math::abs(denom), 0.0000001))
		{
			return false;
		}

		auto t = (D - Math::DotProduct(normal, r.origin())) / denom;
		if (!D3Math::Interval{ tMin,tMax }.contains(t))
		{
			return false;
		}
		auto intersection = r.pointAtScalar(t);

		Math::vec3 planarHitP = intersection - Q;
		float64 alpha = Math::DotProduct(w, Math::CrossProduct(planarHitP, v));
		float64 beta = Math::DotProduct(w, Math::CrossProduct(u, planarHitP));
		if (!isInterior(alpha, beta, rec))
		{
			return false;
		}

		rec.t = t;
		rec.p = intersection;
		rec.mat = material;
		rec.setFaceNormal(r, normal);

		return true;
	}
	bool Quad::isInterior(float64 a, float64 b, HitRecord& rec) const
	{
		D3Math::Interval unitInterval{ 0, 1 };

		if (!unitInterval.contains(a) || !unitInterval.contains(b))
			return false;

		rec.U = a;
		rec.V = b;

		return true;
	}
}