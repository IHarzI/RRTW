// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "RTW_Image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb/stb_image.h"

namespace RTW
{
    RTW_Image::RTW_Image(const char* imageFileName)
    {

		auto fileName = std::string(imageFileName);
		std::string imagesDir = "assets/resourses/images";

		// Hunt for the image file in some likely locations.
		if (load(std::string(imagesDir) + "/" + imageFileName)) return;
		if (load(fileName)) return;
		if (load("images/" + fileName)) return;
		if (load("../images/" + fileName)) return;
		if (load("../../images/" + fileName)) return;
		if (load("../../../images/" + fileName)) return;
		if (load("../../../../images/" + fileName)) return;
		if (load("../../../../../images/" + fileName)) return;
		if (load("../../../../../../images/" + fileName)) return;

		RTW_WARN("Could not load image file %s", fileName.c_str());
    }

	RTW_Image::~RTW_Image()
	{
		STBI_FREE(fdata);
	}

	bool RTW_Image::load(const std::string& filePath)
	{
		auto n = bytesPerPixel;
		fdata = stbi_loadf(filePath.c_str(), &imageWidth, &imageHeight, &n, bytesPerPixel);
		if (!fdata)
		{
			return false;
		}
		bytesPerBufferColumn = imageWidth * bytesPerPixel;
		contertToBytes();

		return true;
	}
};