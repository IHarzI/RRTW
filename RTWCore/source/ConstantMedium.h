// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RayObject.h"
#include "RTW_Material.h"

namespace RTW
{
	class ConstantMedium : public RayObject
	{
	public:
		ConstantMedium(SharedMemoryHandle<RayObject> boundary, float64 density, SharedMemoryHandle<RTW_Texture> texture)
			: boundary(boundary), negInvDensity(-1.0/density), 
			phaseFunction(MakeUniqueHandle<Materials::Isotropic>(texture).RetrieveResourse()) {}

		ConstantMedium(SharedMemoryHandle<RayObject> boundary, float64 density, const Math::color& albedo)
			: boundary(boundary), negInvDensity(-1.0 / density),
			phaseFunction(MakeUniqueHandle<Materials::Isotropic>(albedo).RetrieveResourse()) {}

		virtual D3Math::AABB boundingBox() const override { return boundary->boundingBox(); }

		bool hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const override;
	private:
		SharedMemoryHandle<RayObject> boundary;
		float64 negInvDensity;
		SharedMemoryHandle<RTW_Material> phaseFunction;
	};

};