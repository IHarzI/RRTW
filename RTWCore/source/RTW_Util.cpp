// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "RTW_Util.h"

namespace RTW
{
	namespace Util
	{
		void writeColor(char* ImageCharBuff, const RTW::Math::color color, const Math::vec2i imageSize, const Math::vec2i PixelCoords)
		{
			const uint32 PixelPos = PixelImageFormatSizes[P3M] * (PixelCoords.x + PixelCoords.y * imageSize.x);
			char* PixelInfoBuf = &ImageCharBuff[PixelPos];
			const char* CharFormating = PixelImageCharFormat[P3M];
			uint8 Red = (uint8)(255.f * Math::clamp(linearToGamma(color.r), .0, 0.999));
			uint8 Green = (uint8)(255.f * Math::clamp(linearToGamma(color.g), .0, 0.999));
			uint8 Blue = (uint8)(255.f * Math::clamp(linearToGamma(color.b), .0, 0.999));
			sprintf(PixelInfoBuf, CharFormating, Red, Green, Blue);
		}
		void writePixelColor(uint32* Pixel, Math::color color)
		{
			*Pixel |= 1 << 24;
			uint8 red = (uint8)(255.f * Math::clamp(linearToGamma(color.r), .0, 0.999));
			uint8 green = (uint8)(255.f * Math::clamp(linearToGamma(color.g), .0, 0.999));
			uint8 blue = (uint8)(255.f * Math::clamp(linearToGamma(color.b), .0, 0.999));
			*Pixel |= red << 16 | green << 8 | blue;
		}
	};

};