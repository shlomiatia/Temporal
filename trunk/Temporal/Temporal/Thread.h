#ifndef THREAD_H
#define THREAD_H

namespace Temporal
{
	class Thread
	{
	public:
		static void sleep(float seconds);
	private:
		Thread() {}
		Thread(const Thread&);
		Thread& operator=(const Thread&);
	};
}
#endif