#pragma once

namespace Temporal
{
	class Thread
	{
	public:
		static void sleep(unsigned int millis);
		static unsigned long ticks(void);
	private:
		Thread(void) {}
		Thread(const Thread&);
		Thread& operator=(const Thread&);
	};
}