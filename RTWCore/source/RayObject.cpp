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
        frontFace = (bool)(Math::less(Math::DotProduct(r.direciton(), outwardNormal), 0.f));
        normal = (bool)frontFace ? outwardNormal : -1.f * outwardNormal;
    }
};