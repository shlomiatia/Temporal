#ifndef LOG_H
#define LOG_H

namespace Temporal
{
	class Log
	{
	public:
		static void init();
		static void write(const char* log, ...);
		static void dispose();
	private:
		Log() {}
		Log(const Log&);
		Log& operator=(const Log&);
	};
}
#endif