// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "RayObject.h"

namespace RTW
{

    void HitRecord::setFaceNormal(const Ray& r, const Math::vec3 outwardNormal)
    {
        frontFace = (bool)(Math::less(Math::DotProduct(r.direction(), outwardNormal), 0.f));
        normal = (bool)frontFace ? outwardNormal : -1.f * outwardNormal;
    }

    bool TranslatedObject::hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const
    {
        Ray offsetRay(r.origin() - offset, r.direction(), r.tm);
        if (!object->hit(offsetRay, tMin, tMax, rec))
        {
            return false;
        }

        rec.p += offset;

        return true;
    }
    bool RotateYOperation::hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const
    {
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cosTheta * r.origin()[0] - sinTheta * r.origin()[2];
        origin[2] = sinTheta * r.origin()[0] + cosTheta * r.origin()[2];

        direction[0] = cosTheta * r.direction()[0] - sinTheta * r.direction()[2];
        direction[2] = sinTheta * r.direction()[0] + cosTheta * r.direction()[2];

        Ray rotatedR{ origin,direction,r.tm };

        if (!object->hit(rotatedR, tMin, tMax, rec))
        {
            return false;
        }

        auto p = rec.p;
        p[0] = cosTheta * rec.p[0] + sinTheta * rec.p[2];
        p[2] = -sinTheta* rec.p[0] + cosTheta * rec.p[2];

        auto normal = rec.normal;
        normal[0] = cosTheta * rec.normal[0] + sinTheta * rec.normal[2];
        normal[2] = -sinTheta * rec.normal[0] + cosTheta * rec.normal[2];

        rec.p = p;
        rec.normal = normal;
        
        return true;
    }
};