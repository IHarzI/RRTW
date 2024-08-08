// RRTW
//Ray-tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//WIP.
#pragma once

#include "RTW_CORE.h"

#include <cstdarg>

#define RTW_LOG_WARN_ENABLED   00000001
#define RTW_LOG_INFO_ENABLED   00000010
#define RTW_LOG_DEBUG_ENABLED  00000100
#define RTW_LOG_TRACE_ENABLED  00001000

#define RTW_LOG_MASK (RTW_LOG_WARN_ENABLED | RTW_LOG_INFO_ENABLED | RTW_LOG_DEBUG_ENABLED | RTW_LOG_TRACE_ENABLED)

#define RTW_LOG_ERROR_OR_FATAL_CODE -999

#if RTW_BUILD_RELEASE == 1

#define RTW_LOG_MASK (RTW_LOG_WARN_ENABLED | RTW_LOG_INFO_ENABLED)

#endif

namespace RTW
{

	enum  rtw_log_levels {
		LOG_LEVEL_FATAL = 1 << 0,
		LOG_LEVEL_ERROR = 1 << 1,
		LOG_LEVEL_WARN = 1 << 2,
		LOG_LEVEL_INFO = 1 << 3,
		LOG_LEVEL_DEBUG = 1 << 4,
		LOG_LEVEL_TRACE = 1 << 5
	};

	class Logger
	{
		Logger() {};
		~Logger() {};
	public:
		RTW_STATIC Logger& instance() { RTW_STATIC Logger instance; return instance; }

		RTW_STATIC int logOutput(rtw_log_levels level, const char* msg, ...);

		RTW_STATIC void reportFailedAssert(const char* expression, const char* msg, const char* file, int line)
		{
			logOutput(rtw_log_levels::LOG_LEVEL_FATAL, "Assertion failure: %s, message: %s, in file: %s, line: %d\n", expression, msg, file, line);
		}
	};

};


// logs fatal message
#define RTW_FATAL(msg,...) {RTW::Logger::logOutput(RTW::rtw_log_levels::LOG_LEVEL_FATAL,msg,##__VA_ARGS__); RTW_DEBUG_BREAK();};

// ERROR msg
#ifndef RTW_ERROR
// logs ERROR message
#define RTW_ERROR(msg,...) {RTW::Logger::logOutput(RTW::rtw_log_levels::LOG_LEVEL_ERROR,msg,##__VA_ARGS__); RTW_DEBUG_BREAK();};
#endif // !PH_ERROR

// WARN msg
#if (RTW_LOG_MASK & RTW_LOG_WARN_ENABLED)
// logs WARN message
#define RTW_WARN(msg,...)  RTW::Logger::logOutput(RTW::rtw_log_levels::LOG_LEVEL_WARN,msg,##__VA_ARGS__);
#else
// nothing if WARN isn't enables
#define RTW_WARN(msg,...)
#endif

// INFO msg
#if (RTW_LOG_MASK & RTW_LOG_INFO_ENABLED)
// logs INFO message
#define RTW_INFO(msg,...)  RTW::Logger::logOutput(RTW::rtw_log_levels::LOG_LEVEL_INFO,msg,##__VA_ARGS__);
#else
// nothing if INFO isn't enables
#define RTW_INFO(msg,...)
#endif

// DEBUG msg
#if (RTW_LOG_MASK & RTW_LOG_DEBUG_ENABLED)
// log DEBUG message
#define RTW_DEBUG(msg,...)  RTW::Logger::logOutput(RTW::rtw_log_levels::LOG_LEVEL_DEBUG,msg,##__VA_ARGS__);
#else
// nothing if DEBUG isn't enables
#define RTW_DEBUG(msg,...)
#endif

// TRACE msg
#if (RTW_LOG_MASK & RTW_LOG_TRACE_ENABLED)
// log TRACE message
#define RTW_TRACE(msg,...)  RTW::Logger::logOutput(RTW::rtw_log_levels::LOG_LEVEL_TRACE,msg,##__VA_ARGS__);
#else
// nothing if TRACE isn't enables
#define RTW_TRACE(msg,...)
#endif

// '''''''''''''''''''''''''''''''''''''''''''''''''
// ASSERT ------------------------------------------

#define RTW_ASSERTIONS_ENABLED 1

#ifdef RTW_ASSERTIONS_ENABLED

#if _MSC_VER
#include <intrin.h>
#define RTW_DEBUG_BREAK() __debugbreak()
#else
#define RTW_DEBUG_BREAK() __builtin_trap()
#endif

#define RTW_ASSERT(expr)																\
	if(!(bool)(expr)){																	\
		RTW::Logger::reportFailedAssert(#expr, "", __FILE__, __LINE__);					\
		RTW_DEBUG_BREAK();																\
	};													

#define RTW_ASSERT_MSG(expr,msg)														\
	if (!(bool)(expr)) {																\
																						\
			RTW::Logger::reportFailedAssert(#expr, msg, __FILE__, __LINE__);			\
			RTW_DEBUG_BREAK();															\
	};

#ifdef RTW_DEBUG
#define RTW_DEBUG_ASSERT(expr)															\
	if (!(bool)(expr)) {																\
																						\
			RTW::Logger::reportFailedAssert(#expr, "", __FILE__, __LINE__);				\
			RTW_DEBUG_BREAK();															\
	};

#define RTW_DEBUG_ASSERT_MSG(expr,msg)													\
	if (!(bool)(expr)) {																\
			RTW::Logger::reportFailedAssert(#expr, msg, __FILE__, __LINE__);			\
			RTW_DEBUG_BREAK();															\
	};

#else
#define RTW_DEBUG_ASSERT(expr)
#define RTW_DEBUG_ASSERT_MSG(expr,msg)
#endif // RTW_DEBUG

#else

#define RTW_ASSERT(expr)		
#define RTW_ASSERT_MSG(expr, msg)
#define RTW_DEBUG_ASSERT(expr)
#define RTW_DEBUG_ASSERT_MSG(expr,msg)

#endif // RTW_ASSERTIONS_ENABLED