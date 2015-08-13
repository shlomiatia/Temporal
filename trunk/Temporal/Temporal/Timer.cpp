#include "Timer.h"
#include "Log.h"
#include <SDL.h>

namespace Temporal
{
	float Time::now()
	{
		unsigned long ticks = SDL_GetTicks();
		return ticks / 1000.0f;
	}

	void PerformanceTimer::measure()
	{
		_last = Time::now();
	}

	void PerformanceTimer::print(const char* name, int maxSplits)
	{
		if(_last == 0.0f)
			return;
		++_splits;
		_total += (Time::now() - _last);
		if(_splits == maxSplits)
		{
			float fps = _splits / _total;
			float average = (_total / _splits) * 1000.0f;
			//Log::write("%s: %f FPS (%f seconds)", name, fps, average);
			_splits = 0;
			_total = 0.0f;
		}
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