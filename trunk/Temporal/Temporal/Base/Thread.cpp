#include "Thread.h"
#include <SDL.h>

namespace Temporal
{
	void Thread::sleep(unsigned int millis)
	{
		SDL_Delay(millis);
	}

	unsigned long Thread::getElapsedTimeInMillis(void)
	{
		return SDL_GetTicks();
	}
}