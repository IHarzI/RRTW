// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "ConstantMedium.h"

namespace RTW
{
    bool ConstantMedium::hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const
    {
        HitRecord rec1, rec2;

        if (!boundary->hit(r, D3Math::interval_universe.min, D3Math::interval_universe.max, rec1))
            return false;

        if (!boundary->hit(r, rec1.t + 0.0001, Math::infinity<double>(), rec2))
            return false;

        if (rec1.t < tMin) rec1.t = tMin;
        if (rec2.t > tMax) rec2.t = tMax;

        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        auto rayLength = r.direction().Lenght();
        auto distanceInsideBoundary = (rec2.t - rec1.t) * rayLength;
        auto hitDistance = negInvDensity * Math::log(Util::randomDouble());

        if (hitDistance > distanceInsideBoundary)
            return false;

        rec.t = rec1.t + hitDistance / rayLength;
        rec.p = r.pointAtScalar(rec.t);

        rec.normal = Math::vec3{1,0,0};
        rec.frontFace = true;
        rec.mat = phaseFunction;

        return true;
    }
};
