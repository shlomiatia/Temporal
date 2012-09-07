#ifndef TIMER_H
#define TIMER_H

namespace Temporal
{
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
}
#endif