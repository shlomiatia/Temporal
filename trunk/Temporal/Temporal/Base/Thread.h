#pragma once

namespace Temporal
{
	class Thread
	{
	public:
		static void sleep(unsigned long millis);
		static long ticks(void);
	private:
		Thread(void) {}
		Thread(const Thread&);
		Thread& operator=(const Thread&);
	};
}