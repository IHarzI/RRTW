// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RTW_MATH.h"
#include "RTW_Memory.h"
#include "RTW_Image.h"
#include "Perlin.h"

namespace RTW
{
	class RTW_Texture
	{
	public:
		virtual ~RTW_Texture() = default;

		virtual Math::color value(float64 U, float64 V, const Math::vec3& p) const = 0;
	};

	namespace Textures
	{
		class SolidColor : public RTW_Texture
		{
		public:
			SolidColor(const Math::color& albedo) : albedo(albedo) {};

			SolidColor(float32 R, float32 G, float32 B) : albedo({ R,G,B }) {};

			Math::color value(float64 U, float64 V, const Math::vec3& p) const override
			{
				return albedo;
			}

		private:
			Math::color albedo;
		};

		class CheckerTexture : public RTW_Texture
		{
		public: 
			CheckerTexture(float64 scale, SharedMemoryHandle<RTW_Texture> even, SharedMemoryHandle<RTW_Texture> odd)
				: invScale(1.0 / scale), even(even), odd(odd) {};

			CheckerTexture(float64 scale, Math::color even, Math::color odd)
				: invScale(1.0 / scale), even(rtw_new<SolidColor>(even)), odd(rtw_new<SolidColor>(odd)) {};

			Math::color value(float64 U, float64 V, const Math::vec3& p) const override;

		private:
			float64 invScale;
			SharedMemoryHandle<RTW_Texture> even;
			SharedMemoryHandle<RTW_Texture> odd;
		};

		class ImageTexture : public RTW_Texture
		{
		public:
			ImageTexture(const char* filename) : image(filename) {};

			Math::color value(float64 U, float64 V, const Math::vec3& p) const override;

		private:
			RTW_Image image;
		};

		class NoiseTexture : public RTW_Texture
		{
		public:
			NoiseTexture(float64 scale = 1.0): scale(scale) {};

			Math::color value(float64 U, float64 V, const Math::vec3& p) const override;

		private:
			Perlin noise;
			float64 scale;
		};
	}

};