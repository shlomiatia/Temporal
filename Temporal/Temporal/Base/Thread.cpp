#include "Thread.h"
#include <SDL.h>

namespace Temporal
{
	void Thread::sleep(const long millis)
	{
		SDL_Delay(millis);
	}

	long Thread::ticks(void)
	{
		return SDL_GetTicks();
	}
}