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
	}

};
