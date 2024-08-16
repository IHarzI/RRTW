// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include <Ray.h>
#include "RTW_Memory.h"
#include "RTW_Material.h"
#include "AABB.h"

namespace RTW
{
	struct HitRecord {
		float64 t;
		float64 U;
		float64 V;
		WeakMemoryHandle<RTW_Material> mat;
		Math::vec3 p;
		Math::vec3 normal;
		uint8 frontFace : 1;

		void setFaceNormal(const Ray& r, const Math::vec3 outwardNormal);
	};

	class RayObject
	{
	public:
		virtual bool hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const = 0;

		virtual D3Math::AABB boundingBox() const = 0;

		virtual float64 pdf_value(const Math::vec3& origin, const Math::vec3& direction) const {
			return 0.0;
		}

		virtual Math::vec3 random(const Math::vec3& origin) const {
			return { 1, 0, 0 };
		}
	};

	class TranslatedObject : public RayObject
	{
	public:

		TranslatedObject(SharedMemoryHandle<RayObject> object, const Math::vec3 offset)
			: object(object), offset(offset)
		{
			bBox = object->boundingBox() + offset;
		}

		bool hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const override;

		D3Math::AABB boundingBox() const override {
			return bBox;
		}

	private:
		SharedMemoryHandle<RayObject> object;
		Math::vec3 offset;
		D3Math::AABB bBox;
	};

	class RotateYOperation : public RayObject
	{
	public:

		RotateYOperation(SharedMemoryHandle<RayObject> object, float64 angle)
			: object(object)
		{
			auto radians = Math::radians(angle);
			sinTheta = Math::sin(radians);
			cosTheta = Math::cos(radians);
			bBox = object->boundingBox();

			Math::vec3 min{ Math::infinity<float64>() };
			Math::vec3 max{ -Math::infinity<float64>() };

			for (int32 i = 0; i < 2; i++)
			{
				for (int32 j = 0; j < 2; j++)
				{
					for (int32 k = 0; k < 2; k++)
					{
						auto x = i * bBox.x.max + (1 - i) * bBox.x.min;
						auto y = j * bBox.y.max + (1 - j) * bBox.y.min;
						auto z = k * bBox.z.max + (1 - k) * bBox.z.min;

						auto newX = cosTheta * x + sinTheta * z;
						auto newZ = -sinTheta * x + cosTheta * z;

						Math::vec3 tester{ newX, y, newZ };

						for (int32 c = 0; c < 3; c++)
						{
							min[c] = Math::min(min[c], tester[c]);
							max[c] = Math::max(max[c], tester[c]);
						}

					}
				}
			}

			bBox = D3Math::AABB{ min, max };
		}

		bool hit(const Ray& r, float64 tMin, float64 tMax, HitRecord& rec) const override;

		D3Math::AABB boundingBox() const override {
			return bBox;
		}

	private:
		SharedMemoryHandle<RayObject> object;
		float64 sinTheta;
		float64 cosTheta;
		D3Math::AABB bBox;
	};

};