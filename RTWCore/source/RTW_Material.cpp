// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image.
//WIP.
#include "RTW_Material.h"
#include "RayObject.h"

namespace RTW {

	namespace Materials {

		bool Lambertian::scatter(const Ray* rayIn, const HitRecord* rec, Math::vec3& attenuation, Ray* scattered) const
		{
			Math::vec3 ScatterDirection = rec->normal + Util::RandomUnitSphereUnitVector();

			if (ScatterDirection.NearZero())
			{
				ScatterDirection = rec->normal;
			}

			*scattered = Ray(rec->p, ScatterDirection, rayIn->tm);
			attenuation = albedo;
			return true;
		}

		bool Metal::scatter(const Ray* rayIn, const HitRecord* rec, Math::vec3& attenuation, Ray* scattered) const
		{
			Math::vec3 Reflected = rayIn->direciton().Reflect(rec->normal);
			Reflected = Reflected.GetNormalized() + (Util::RandomUnitSphereUnitVector() * Fuzz);
			*scattered = Ray(rec->p, Reflected, rayIn->tm);
			attenuation = albedo;
			return Math::more(scattered->direciton().DotProduct(rec->normal), 0);
		}
		bool Dielectric::scatter(const Ray* rayIn, const HitRecord* rec, Math::vec3& attenuation, Ray* scattered) const
		{
			attenuation = { 1.f};
			float64 ri = rec->frontFace ? (1.0 / refractionIndex) : refractionIndex;
			Math::vec3 normalizedDirection = rayIn->direciton().Normalize();

			double cosTheta = Math::min((double)(normalizedDirection * -1.f).DotProduct(rec->normal), 1.0);
			double sinTheta = Math::sqrt(1.0 - cosTheta * cosTheta);

			bool cannotRefract = (ri * sinTheta) > 1.0;
			Math::vec3 Direction;

			if (cannotRefract || Math::reflectance(cosTheta,ri) > Util::randomDouble())
			{
				Direction = normalizedDirection.Reflect(rec->normal);
			}
			else
			{
				Direction = normalizedDirection.Refract(rec->normal, ri);
			}

			*scattered = Ray(rec->p, Direction, rayIn->tm);
			return true;
		}
	}
}