// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing in One Weekend.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image(not multhithreaded).
//WIP.

#include "RTW_Util.h"

namespace RTW
{
	namespace Util
	{
		void writeColor(char* ImageCharBuff, const RTW::Math::vec3 color, const Math::vec2i imageSize, const Math::vec2i PixelCoords)
		{
			const uint32 PixelPos = PixelImageFormatSizes[P3M] * (PixelCoords.x + PixelCoords.y * imageSize.x);
			char* PixelInfoBuf = &ImageCharBuff[PixelPos];
			const char* CharFormating = PixelImageCharFormat[P3M];
			uint8 Red = (uint8)(255.f * Math::clamp(linearToGamma(color.r), 0.f, 0.999f));
			uint8 Green = (uint8)(255.f * Math::clamp(linearToGamma(color.g), 0.f, 0.999f));
			uint8 Blue = (uint8)(255.f * Math::clamp(linearToGamma(color.b), 0.f, 0.999f));
			sprintf(PixelInfoBuf, CharFormating, Red, Green, Blue);
		}
		void writePixelColor(uint32* Pixel, Math::vec3 color)
		{
			*Pixel |= 1 << 24;
			uint8 red = (uint8)(255.f * Math::clamp(linearToGamma(color.r), 0.f, 0.999f));
			uint8 green = (uint8)(255.f * Math::clamp(linearToGamma(color.g), 0.f, 0.999f));
			uint8 blue = (uint8)(255.f * Math::clamp(linearToGamma(color.b), 0.f, 0.999f));
			*Pixel |= red << 16 | green << 8 | blue;
		}
	};

};