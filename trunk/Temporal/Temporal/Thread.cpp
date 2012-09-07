#include "Thread.h"
#include <SDL.h>

namespace Temporal
{
	void Thread::sleep(float seconds)
	{
		unsigned int millis = static_cast<unsigned int>(seconds * 1000.0f);
		SDL_Delay(millis);
	}

	float Thread::getElapsedTime()
	{
		unsigned long millis = SDL_GetTicks();
		return millis / 1000.0f;
	}
}