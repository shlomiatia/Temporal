#ifndef THREAD_H
#define THREAD_H

namespace Temporal
{
	class Thread
	{
	public:
		static void sleep(unsigned int millis);
		static unsigned long getElapsedTimeInMillis();
	private:
		Thread() {}
		Thread(const Thread&);
		Thread& operator=(const Thread&);
	};
}
#endif