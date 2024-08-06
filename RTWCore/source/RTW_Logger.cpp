// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image.
//WIP.
#include "RTW_Logger.h"

#include <memory>

namespace RTW
{

	int logLevelEnumToIndex(rtw_log_levels level)
	{
		int index = 0;
		if ((int)level == 0)
			return 0;
		else
		{
			while (((int)level >> index) != 1)
			{
				index++;
				if (index > sizeof(int) * 8)
					return -1;
			}

			return index;
		}
		return -1;
	}

	int Logger::logOutput(rtw_log_levels level, const char* msg, ...)
	{
		static const char* levels[6] = { "[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: " };
		// check if it's FATAL or ERROR
		bool is_error = (level & rtw_log_levels::LOG_LEVEL_FATAL | level & rtw_log_levels::LOG_LEVEL_ERROR);

		const int msg_size = 12000;
		char raw_out_msg[msg_size];
		memset(raw_out_msg, 0, sizeof(raw_out_msg));

		char* arg_ptr;
		va_start(arg_ptr, msg);
		vsnprintf(raw_out_msg, msg_size, msg, arg_ptr);
		va_end(arg_ptr);

		char out_msg[msg_size];
		int LogIndex = logLevelEnumToIndex(level);
		sprintf(out_msg, "%s%s\n", levels[LogIndex], raw_out_msg);

		printf("%s", out_msg);

		if (is_error)
			return RTW_LOG_ERROR_OR_FATAL_CODE;

		return 1;
	}
};