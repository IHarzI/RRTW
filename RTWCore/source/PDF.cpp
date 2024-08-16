// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#include "PDF.h"

#include "RayList.h"

namespace RTW
{

	namespace PDFs
	{
		float64 HittablePDF::value(const Math::vec3& Direction) const
		{
			return objects->pdf_value(origin, Direction);
		}

		Math::vec3 HittablePDF::generate() const
		{
			return objects->random(origin);
		}

		float64 MixturePDF::value(const Math::vec3& Direction) const
		{
			return 0.5 * pdfs[0]->value(Direction) + 0.5 * pdfs[1]->value(Direction);
		}

		Math::vec3 MixturePDF::generate() const
		{
			if (Util::randomDouble() < 0.5)
			{
				return pdfs[0]->generate();
			}
			else
			{
				return pdfs[1]->generate();
			}
		}

	};

}