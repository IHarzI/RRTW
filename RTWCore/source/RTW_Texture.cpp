// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "RTW_Texture.h"

namespace RTW
{

	namespace Textures {
		Math::color CheckerTexture::value(float64 U, float64 V, const Math::vec3& p) const
		{
			int32 xInt = (int32)Math::floor(invScale * p.x);
			int32 yInt = (int32)Math::floor(invScale * p.y);
			int32 zInt = (int32)Math::floor(invScale * p.z);

			bool isEven = (xInt + yInt + zInt) % 2 == 0;

			return isEven ? even->value(U,V,p) : odd->value(U,V,p);
		}

		Math::color ImageTexture::value(float64 U, float64 V, const Math::vec3& p) const
		{
			if (image.height() <= 0)
				return { 0, 1, 1 };

			U = Math::clamp(U, 0.0, 1.0);
			V = 1.0 - Math::clamp(V, 0.0, 1.0);

			auto i = int32(U * image.width());
			auto j = int32(V * image.height());
			auto pixel = image.pixelData(i, j);

			auto colorScale = 1.0 / 255.0;
			return Math::color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
		}

		Math::color NoiseTexture::value(float64 U, float64 V, const Math::vec3& p) const
		{
			//auto NoiseColor = Math::color(1.0,1.0,1.0) * 0.5 * (1.0 + noise.noise(scale * p));
			//const int32 Multiplayer = 50;
			//return Math::color{ Math::sin(NoiseColor.x* Multiplayer), Math::sin(NoiseColor.y* Multiplayer), Math::sin(NoiseColor.z* Multiplayer) };
			auto NoiseColor = Math::color(0.5, 0.5, 0.5) * ((1 + Math::sin(scale * p.z + 10 *  noise.turb(p, 15))));
			return NoiseColor;
		}
	}

};
