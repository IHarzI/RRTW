// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "BVHNode.h"
#include "RTW_Util.h"
#include "AABB.h"
#include <algorithm>

namespace RTW
{
    BVH_Node::BVH_Node(const RayList::ObjectList& List, uint64 start, uint64 end)
    {
        bBox = D3Math::AABB_empty;
        for (uint64 objectIndex = start; objectIndex < end; objectIndex++)
        {
            bBox = D3Math::AABB(bBox, List[objectIndex]->boundingBox());
        }

        int32 axis = bBox.longestAxis();

        bool (*comparators[3])(RayList::ObjectHandle, RayList::ObjectHandle) = {
        [](RayList::ObjectHandle a, RayList::ObjectHandle b) -> bool {return BVH_Node::boxCompare(a,b,0); } ,
        [](RayList::ObjectHandle a, RayList::ObjectHandle b) -> bool {return BVH_Node::boxCompare(a,b,1); } ,
        [](RayList::ObjectHandle a, RayList::ObjectHandle b) -> bool {return BVH_Node::boxCompare(a,b,2); } };

        uint64 objectSpan = end - start;

        if (objectSpan == 1)
        {
            left = right = List[start];
        }
        else if (objectSpan == 2)
        {
            left = List[start];
            right = List[start + 1];
        }
        else
        {
            std::sort(std::begin(List) + start, std::begin(List) + end, comparators[axis]);

            auto mid = start + objectSpan / 2;
            left = rtw_new<BVH_Node>(List, start, mid);
            right = rtw_new<BVH_Node>(List, mid, end);
        }
    }

    bool BVH_Node::hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const
    {
        if (!bBox.overlap(r, { tMin,tMax }))
        {
            return false;
        }
        bool hitLeft = left->hit(r, tMin, tMax, rec);
        bool hitRight = right->hit(r, tMin, hitLeft? rec.t : tMax, rec);
        return hitLeft || hitRight;
    }
};