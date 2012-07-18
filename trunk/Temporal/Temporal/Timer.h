#ifndef TIMER_H
#define TIMER_H

namespace Temporal
{
	class Timer
	{
	public:
		Timer() : _elapsedTimeInMillis(0.0f) {}

		float getElapsedTimeInMillis() const { return _elapsedTimeInMillis; }
		void reset() { _elapsedTimeInMillis = 0.0f; }
		void reset(float elapsedTimeInMillis) { _elapsedTimeInMillis = elapsedTimeInMillis; }
		void update(float updateTimeInMillis) { _elapsedTimeInMillis += updateTimeInMillis; }

	private:
		float _elapsedTimeInMillis;

		Timer(const Timer&);
		Timer& operator=(const Timer&);
	};
}
#endif