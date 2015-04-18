#include "Thread.h"
#include <SDL.h>
#include <SDL_thread.h>

namespace Temporal
{
	void Thread::sleep(float seconds)
	{
		unsigned int millis = static_cast<unsigned int>(seconds * 1000.0f);
		SDL_Delay(millis);
	}

	void Thread::start(int (*function)(void*), void *data)
	{
		_thread = SDL_CreateThread(function, "MyThread", data);
	}

	Thread::~Thread()
	{
		SDL_WaitThread(_thread, 0);
	}

	Semaphore::Semaphore()
	{
		_semaphore = SDL_CreateSemaphore(0);
	}

	Semaphore::~Semaphore()
	{
		SDL_DestroySemaphore(_semaphore);
	}

	void Semaphore::wait()
	{
		SDL_SemWait(_semaphore);
	}

	void Semaphore::notify()
	{
		SDL_SemPost(_semaphore);
	}
}