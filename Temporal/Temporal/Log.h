#ifndef LOG_H
#define LOG_H

namespace Temporal
{
	class Log
	{
	public:
		static void write(const char* log, ...);
	private:
		Log() {}
		Log(const Log&);
		Log& operator=(const Log&);
	};
}
#endif