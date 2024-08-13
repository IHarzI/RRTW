// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "Sphere.h"

namespace RTW
{
	bool Sphere::hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const
	{
		Math::vec3 realCenter = isMoving ? CalculateCenterFromTime(r.tm) : center;
		Math::vec3 oc = r.origin() - realCenter;
		float64 a = Math::DotProduct(r.direction(), r.direction());
		float64 b = Math::DotProduct(oc, r.direction());
		float64 c = Math::DotProduct(oc, oc) - radius * radius;
		float64 discrimant = b * b - a * c;
		if (Math::more(discrimant,0.0))
		{
			float64 temp = (-b - Math::sqrt(b * b - a * c)) / a;
			bool hit = false;
			if (temp < tMax && temp > tMin)
			{
				hit = true;
			}

			if (!hit)
			{
				temp = (-b + Math::sqrt(b * b - a * c)) / a;
				if (temp < tMax && temp > tMin)
				{
					hit = true;
				}
			};

			if (hit)
			{
				rec.t = temp;
				rec.p = r.pointAtScalar(rec.t);
				Math::vec3 outwardNormal = (rec.p - center) / radius;
				rec.setFaceNormal(r, outwardNormal);
				getSphereUV(outwardNormal, rec.U, rec.V);
				rec.mat = material;
				return true;
			}
		}

		return false;
	}
}