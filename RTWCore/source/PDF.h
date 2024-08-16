// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RTW_Util.h"
#include "RTW_Memory.h"

namespace RTW
{
	class RayObject;

	class PDF
	{
	public:
		virtual ~PDF() {};

		virtual float64 value(const Math::vec3& Direction) const = 0;
		virtual Math::vec3 generate() const = 0;
	};

	namespace PDFs
	{
		class SpherePDF : public PDF
		{
		public:
			SpherePDF() {}

			RTW_INLINE float64 value(const Math::vec3& Direction) const override
			{
				return 1.0 / (4.0 * Math::pi<float64>());
			}

			RTW_INLINE Math::vec3 generate() const override
			{
				return Util::RandomUnitSphereUnitVector();
			}
		};

		class CosinePDF : public PDF
		{
		public:
			CosinePDF(const Math::vec3& w) : uvw(w) {};

			RTW_INLINE float64 value(const Math::vec3& Direction) const override
			{
				auto cosineTheta = Math::DotProduct(Direction.GetNormalized(), uvw.w());
				return Math::max(0, cosineTheta / Math::pi<float64>());
			}

			RTW_INLINE Math::vec3 generate() const override
			{
				return uvw.transform(Util::RandomCosineDirection());
			}

		private:
			Util::ONB uvw;
		};

		class HittablePDF : public PDF
		{
		public:
			HittablePDF(const RayObject* objects, const Math::vec3 origin) 
			: objects(objects), origin(origin) {}

			float64 value(const Math::vec3& Direction) const override;

			Math::vec3 generate() const override;
		private:
			const RayObject* objects;
			Math::vec3 origin;
		};

		class MixturePDF : public PDF
		{
		public:
			MixturePDF(SharedMemoryHandle<PDF> p0, SharedMemoryHandle<PDF> p1)
			{
				pdfs[0] = p0; pdfs[1] = p1;
			}

			float64 value(const Math::vec3& Direction) const override;

			Math::vec3 generate() const override;
		private:
			SharedMemoryHandle<PDF> pdfs[2];
		};
	}
};