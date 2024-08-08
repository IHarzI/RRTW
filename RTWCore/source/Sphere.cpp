// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "Sphere.h"

namespace RTW
{
	bool Sphere::hit(const Ray& r, float32 tMin, float32 tMax, HitRecord& rec) const
	{
		Math::vec3 realCenter = isMoving ? CalculateCenterFromTime(r.tm) : center;
		Math::vec3 oc = r.origin() - realCenter;
		float32 a = Math::DotProduct(r.direciton(), r.direciton());
		float32 b = Math::DotProduct(oc, r.direciton());
		float32 c = Math::DotProduct(oc, oc) - radius * radius;
		float32 discrimant = b * b - a * c;
		if (Math::more(discrimant,0.f))
		{
			float temp = (-b - Math::sqrt(b * b - a * c)) / a;
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