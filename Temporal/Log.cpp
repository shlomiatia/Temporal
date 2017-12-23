#include "Log.h"
#include "Timer.h"
#include "FileStream.h"
#include <cstdarg>
#include <stdio.h>

namespace Temporal
{
	static FileStream _fs("log.txt", true, false);

	void Log::init()
	{

	}

	void Log::write(const char* text, ...)
	{
		std::string buffer;
		char buffer2[1000];
		sprintf(buffer2, "[%0.2f] ", Time::now());
		buffer.append(buffer2);
		va_list arguments;
		va_start(arguments, text);
		vsprintf(buffer2, text, arguments);
		buffer.append(buffer2);
		va_end(arguments);
		buffer.append("\n");
		printf(buffer.c_str());
		//_fs.write(buffer.c_str());
	}
	
	void Log::dispose()
	{
		_fs.close();
	}
}
