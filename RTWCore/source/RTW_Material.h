// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RTW_MATH.h"
#include "RTW_Util.h"

namespace RTW
{
	class Ray;
	struct HitRecord;

	class Material
	{
	public:
		virtual ~Material() = default;

		virtual bool scatter(const Ray* rayIn, const HitRecord* rec, Math::vec3& attenuation, Ray* scattered) const
		{
			return false;
		}
	};

	namespace Materials
	{
		class Lambertian : public Material
		{
		public:
			Lambertian(const Math::vec3& albedo) : albedo(albedo) {};

			bool scatter(const Ray* rayIn, const HitRecord* rec, Math::vec3& attenuation, Ray* scattered) const override;
		private:
			Math::vec3 albedo;
		};

		class Metal : public Material
		{
		public:
			Metal(const Math::vec3& albedo, float64 FuzzFactor) : albedo(albedo), Fuzz(FuzzFactor < 1 ? FuzzFactor : 1) {};

			bool scatter(const Ray* rayIn, const HitRecord* rec, Math::vec3& attenuation, Ray* scattered) const override;
		private:
			Math::vec3 albedo;
			float64 Fuzz;
		};

		class Dielectric : public Material
		{
		public:
			Dielectric(float64 refractionIndex) : refractionIndex(refractionIndex) {};
			
			bool scatter(const Ray* rayIn, const HitRecord* rec, Math::vec3& attenuation, Ray* scattered) const override;
		private:
			float64 refractionIndex;
		};
	};
}