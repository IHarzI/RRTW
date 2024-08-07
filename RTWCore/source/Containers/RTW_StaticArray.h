// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include "RTW_CORE.h"

namespace RTW {
	namespace Containers {
		template <typename ValueT, size_t ArraySize>
		struct PH_StaticArray
		{
			ValueT InternalStorage[ArraySize];

			RTW_INLINE ValueT& operator[](size_t index) { RTW_ASSERT(index < ArraySize); return InternalStorage[index]; }
			RTW_INLINE const ValueT& operator[](size_t index) const { RTW_ASSERT(index < ArraySize); return InternalStorage[index]; }

			RTW_INLINE constexpr ValueT* data() noexcept { return InternalStorage; };
			RTW_INLINE constexpr const ValueT* data() const noexcept { return InternalStorage; };

			RTW_INLINE constexpr ValueT* begin()	const { return InternalStorage; };
			RTW_INLINE constexpr ValueT* end()	const { return InternalStorage + ArraySize; };

			RTW_INLINE constexpr ValueT* begin() { return InternalStorage; };
			RTW_INLINE constexpr ValueT* end() { return InternalStorage + ArraySize; };

			RTW_INLINE constexpr size_t size() const { return ArraySize; };
		};

	}
};

