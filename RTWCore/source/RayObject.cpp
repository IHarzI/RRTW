// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image.
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