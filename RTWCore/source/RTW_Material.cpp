// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "RTW_Material.h"
#include "RayObject.h"

namespace RTW {

	namespace Materials {

		bool Lambertian::scatter(const Ray* rayIn, const HitRecord* rec, ScatterRecord& srec)  const
		{
			srec.attenuation = tex->value(rec->U, rec->V, rec->p);
			srec.pdf = MakeUniqueHandle<PDFs::CosinePDF>(rec->normal).RetrieveResourse();
			srec.skipOff = false;
			return true;
		}

		float64 Lambertian::scattering_pdf(const Ray* r_in, const HitRecord* rec, const Ray* scattered) const
		{
			float64 cos_theta = Math::DotProduct(rec->normal, scattered->direction().Normalize());
			return cos_theta < 0 ? 0 : cos_theta / Math::pi<float64>();
		}

		bool Metal::scatter(const Ray* rayIn, const HitRecord* rec, ScatterRecord& srec)  const
		{
			Math::vec3 Reflected = rayIn->direction().Reflect(rec->normal);
			Reflected = Reflected.GetNormalized() + (Util::RandomUnitSphereUnitVector() * Fuzz);
			
			srec.attenuation = albedo;
			srec.pdf = nullptr;
			srec.skipOff = true;
			srec.skipPdfRay = Ray(rec->p, Reflected, rayIn->tm);

			return true;
		}
		bool Dielectric::scatter(const Ray* rayIn, const HitRecord* rec, ScatterRecord& srec)  const
		{
			srec.attenuation = { 1,1,1 };
			srec.pdf = nullptr;
			srec.skipOff = true;
			float64 ri = rec->frontFace ? (1.0 / refractionIndex) : refractionIndex;
			Math::vec3 normalizedDirection = rayIn->direction().Normalize();

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

			srec.skipPdfRay = Ray(rec->p, Direction, rayIn->tm);
			return true;
		}

		Math::color DiffuseLight::emit(const Ray* rayIn, const HitRecord* rec, float64 u, float64 v, const Math::vec3& p) const
		{
			if (!rec->frontFace)
				return { 0.0 };
			return tex->value(u, v, p);
		}
		bool Isotropic::scatter(const Ray* rayIn, const HitRecord* rec, ScatterRecord& srec)  const
		{
			srec.attenuation = tex->value(rec->U, rec->V, rec->p);
			srec.pdf = MakeUniqueHandle<PDFs::SpherePDF>().RetrieveResourse();
			srec.skipOff = false;
			return true;
		}
		float64 Isotropic::scattering_pdf(const Ray* r_in, const HitRecord* rec, const Ray* scattered) const
		{
			return 1 / (4 * Math::pi<float64>());
		}
	}
}