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

namespace RTW
{
	class Ray;
	struct HitRecord;

	class Material
	{
	public:
		virtual ~Material() = default;

		virtual bool scatter(const Ray* rayIn, const HitRecord* rec, Math::color& attenuation, Ray* scattered) const
		{
			return false;
		}
	};

	namespace Materials
	{
		class Lambertian : public Material
		{
		public:
			Lambertian(const Math::color& albedo) : tex(rtw_new<Textures::SolidColor>(albedo)) {};
			Lambertian(SharedMemoryHandle<RTW_Texture> tex) : tex(tex) {};

			bool scatter(const Ray* rayIn, const HitRecord* rec, Math::color& attenuation, Ray* scattered) const override;
		private:
			SharedMemoryHandle<RTW_Texture> tex;
		};

		class Metal : public Material
		{
		public:
			Metal(const Math::color& albedo, float64 FuzzFactor) : albedo(albedo), Fuzz(FuzzFactor < 1 ? FuzzFactor : 1) {};

			bool scatter(const Ray* rayIn, const HitRecord* rec, Math::color& attenuation, Ray* scattered) const override;
		private:
			Math::color albedo;
			float64 Fuzz;
		};

		class Dielectric : public Material
		{
		public:
			Dielectric(float64 refractionIndex) : refractionIndex(refractionIndex) {};
			
			bool scatter(const Ray* rayIn, const HitRecord* rec, Math::color& attenuation, Ray* scattered) const override;
		private:
			float64 refractionIndex;
		};
	};
}