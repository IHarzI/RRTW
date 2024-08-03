#pragma once

#ifdef RTW_SYSTEM_WINDOWS
#include <Windows.h>
#endif

// Undef Windows stuff that conflicts
#ifdef _WIN32
#undef min 
#undef max 
#undef ERROR 
#undef DELETE 
#undef MessageBox 
#undef Error
#undef OK
#undef CONNECT_DEFERRED 
#endif

#undef ABS
#undef SIGN
#undef MIN
#undef MAX
#undef CLAMP

#define RTW_STATIC static

#define RTW_INLINE inline