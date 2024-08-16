// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RTW_MATH.h"
#include "RTW_Util.h"
#include "RTW_Texture.h"
#include "PDF.h"
#include "Ray.h"

namespace RTW
{
	struct HitRecord;
	struct ScatterRecord
	{
		Math::color attenuation;
		SharedMemoryHandle<PDF> pdf;
		bool skipOff;
		Ray skipPdfRay;
	};

	class RTW_Material
	{
	public:
		virtual ~RTW_Material() = default;

		virtual bool scatter(const Ray* rayIn, const HitRecord* rec, ScatterRecord& srec) const
		{
			return false;
		}

		virtual Math::color emit(const Ray* rayIn, const HitRecord* rec, float64 u, float64 v, const Math::vec3& p) const
		{
			return Math::color{ 0,0,0 };
		}

		virtual float64 scattering_pdf(const Ray* r_in, const HitRecord* rec, const Ray* scattered)	const
		{
			return 0;
		}
	};

	namespace Materials
	{
		class Lambertian : public RTW_Material
		{
		public:
			Lambertian(const Math::color& albedo) : tex(rtw_new<Textures::SolidColor>(albedo)) {};
			Lambertian(SharedMemoryHandle<RTW_Texture> tex) : tex(tex) {};

			bool scatter(const Ray* rayIn, const HitRecord* rec, ScatterRecord& srec) const override;

			float64 scattering_pdf(const Ray* r_in, const HitRecord* rec, const Ray* scattered) const override;

		private:
			SharedMemoryHandle<RTW_Texture> tex;
		};

		class Metal : public RTW_Material
		{
		public:
			Metal(const Math::color& albedo, float64 FuzzFactor) : albedo(albedo), Fuzz(FuzzFactor < 1 ? FuzzFactor : 1) {};

			bool scatter(const Ray* rayIn, const HitRecord* rec, ScatterRecord& srec) const override;
		private:
			Math::color albedo;
			float64 Fuzz;
		};

		class Dielectric : public RTW_Material
		{
		public:
			Dielectric(float64 refractionIndex) : refractionIndex(refractionIndex) {};
			
			bool scatter(const Ray* rayIn, const HitRecord* rec, ScatterRecord& srec) const override;
		private:
			float64 refractionIndex;
		};

		class DiffuseLight : public RTW_Material
		{
		public:
			DiffuseLight(const Math::color& emit) : tex(rtw_new<Textures::SolidColor>(emit)) {};
			DiffuseLight(SharedMemoryHandle<RTW_Texture> tex) : tex(tex) {};

			Math::color emit(const Ray* rayIn, const HitRecord* rec, float64 u, float64 v, const Math::vec3& p) const override;
		private:
			SharedMemoryHandle<RTW_Texture> tex;
		};

		class Isotropic : public RTW_Material
		{
		public:
			Isotropic(const Math::color& albedo) : tex(rtw_new<Textures::SolidColor>(albedo)) {};
			Isotropic(SharedMemoryHandle<RTW_Texture> tex) : tex(tex) {};

			bool scatter(const Ray* rayIn, const HitRecord* rec, ScatterRecord& srec) const override;

			float64 scattering_pdf(const Ray* r_in, const HitRecord* rec, const Ray* scattered) const override;
		private:
			SharedMemoryHandle<RTW_Texture> tex;
		};

	};
}