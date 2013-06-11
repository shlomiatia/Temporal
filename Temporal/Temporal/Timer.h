#ifndef TIMER_H
#define TIMER_H

#include "Hash.h"
#include <unordered_map>

namespace Temporal
{
	class Time
	{
	public:
		static float now();
	};

	class Timer
	{
	public:
		Timer() : _elapsedTime(0.0f) {}

		float getElapsedTime() const { return _elapsedTime; }
		void reset() { _elapsedTime = 0.0f; }
		void reset(float seconds) { _elapsedTime = seconds; }
		void update(float seconds) { _elapsedTime += seconds; }

	private:
		float _elapsedTime;

		Timer(const Timer&);
		Timer& operator=(const Timer&);
	};

	class PerformanceTimer
	{
	public:
		PerformanceTimer() : _startTime(0.0f), _endTime(0.0f), _splits(0) {}

		int getSplits() const { return _splits; }
		void split() { ++_splits; }
		bool isStarted() const { return _startTime != 0.0f; }
		void start();
		void stop();
		float getElapsedTime() const;

	private:
		float _startTime;
		float _endTime;
		int _splits;

		PerformanceTimer(const PerformanceTimer&);
		PerformanceTimer& operator=(const PerformanceTimer&);
	};

	typedef std::unordered_map<Hash, PerformanceTimer*> PerformanceTimerCollection;
	typedef PerformanceTimerCollection::const_iterator PerformanceTimerIterator;

	class PerformanceTimerManager
	{
	public:
		static PerformanceTimerManager& get()
		{
			static PerformanceTimerManager instance;
			return instance;
		}

		PerformanceTimer& getTimer(Hash id);

	private:
		PerformanceTimerCollection _timers;

		PerformanceTimerManager() {};
		PerformanceTimerManager(const PerformanceTimerManager&);
		PerformanceTimerManager& operator=(const PerformanceTimerManager&);
	};

		
}
#endif