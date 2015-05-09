#ifndef THREAD_H
#define THREAD_H

struct SDL_Thread;
struct SDL_semaphore;

namespace Temporal
{
	class Thread
	{
	public:
		static void sleep(float seconds);

		Thread() {};
		void start(int (*function)(void*), void *data);
		~Thread();

	private:
		SDL_Thread* _thread;

		Thread(const Thread&);
		Thread& operator=(const Thread&);
	};

	class Semaphore
	{
	public:
		Semaphore();
		~Semaphore();

		void wait();
		void notify();

	private:
		SDL_semaphore* _semaphore;

		Semaphore(const Semaphore&);
		Semaphore& operator=(const Semaphore&);
	};
}
#endif