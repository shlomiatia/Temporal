#include "Timer.h"
#include <SDL.h>

namespace Temporal
{
	float Time::now()
	{
		unsigned long ticks = SDL_GetTicks();
		return ticks / 1000.0f;
	}

	void PerformanceTimer::start()
	{
		_startTime = Time::now();
		_endTime = 0.0f;
		_splits = 0;
	}

	void PerformanceTimer::stop()
	{
		_endTime = Time::now();
	}

	float PerformanceTimer::getElapsedTime() const
	{
		float endTime = _endTime != 0.0f ? _endTime : Time::now();
		return endTime - _startTime;
	}

	PerformanceTimer& PerformanceTimerManager::getTimer(Hash id)
	{
		PerformanceTimerIterator i = _timers.find(id);
		if(i != _timers.end())
		{
			return *(i->second);
		}
		else
		{
			PerformanceTimer* timer = new PerformanceTimer();
			_timers[id] = timer;
			return *timer;
		}
	}
}