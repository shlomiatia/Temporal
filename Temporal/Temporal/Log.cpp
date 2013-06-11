#include "Log.h"
#include "Timer.h"
#include <cstdarg>
#include <stdio.h>

namespace Temporal
{
	void Log::write(const char* text, ...)
	{
		printf("[%0.2f] ", Time::now());
		va_list arguments;
		va_start(arguments, text);
		vprintf(text, arguments);
		va_end(arguments);
	}
}