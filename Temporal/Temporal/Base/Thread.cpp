#include "Thread.h"
#include <SDL.h>

namespace Temporal
{
	void Thread::sleep(unsigned long millis)
	{
		SDL_Delay(millis);
	}

	long Thread::ticks(void)
	{
		return SDL_GetTicks();
	}
}