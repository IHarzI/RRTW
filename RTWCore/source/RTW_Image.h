// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include "RTW_CORE.h"
#include "RTW_Logger.h"
#include "RTW_MATH.h"
#include "Containers/RTW_DynamicArray.h"

#include <cstdlib>
#include <iostream>


namespace RTW
{

	class RTW_Image
	{
	public:
		RTW_Image() {};

		RTW_Image(const char* imageFileName);

		~RTW_Image();

		bool load(const std::string& filePath);

		int32 width() const { return imageWidth; }
		int32 height() const { return imageHeight; }

		RTW_INLINE const uint8* pixelData(int32 x, int32 y) const
		{
			RTW_STATIC uint8 MagentaColor[] = { 255,0,255 };
			if (bdata.Empty()) return MagentaColor;
			x = Math::clamp(x, 0, imageWidth);
			y = Math::clamp(y, 0, imageHeight);
			return bdata.data() + y * bytesPerBufferColumn + x * bytesPerPixel;
		}
	private:

		RTW_STATIC RTW_INLINE uint8 floatToByte(float32 value) {
			if (value <= 0.0)
				return 0;
			if (1.0 <= value)
				return 255;
			return static_cast<uint8>(256.0 * value);
		}

		void contertToBytes()
		{
			int32 totalBytes = imageWidth * imageHeight * bytesPerPixel;
			bdata.Resize(totalBytes);

			auto* bptr = bdata.data();
			auto* fptr = fdata;
			for (auto i = 0; i < totalBytes; i++, fptr++, bptr++)
			{
				*bptr = floatToByte(*fptr);
			}

		}

		const int32 bytesPerPixel = 3;
		float32* fdata = nullptr;
		Containers::DynamicArray<uint8> bdata{};
		int32 imageWidth = 0;
		int32 imageHeight = 0;
		int32 bytesPerBufferColumn = 0;
	};

};