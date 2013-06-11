#include "Thread.h"
#include <SDL.h>

namespace Temporal
{
	void Thread::sleep(float seconds)
	{
		unsigned int millis = static_cast<unsigned int>(seconds * 1000.0f);
		SDL_Delay(millis);
	}
}