#pragma once

namespace Temporal
{
	// TODO: Take update time from central location SLOTH!
	class Timer
	{
	public:
		Timer(void) : _elapsedTimeInMillis(0.0f) {}

		float getElapsedTimeInMillis(void) const { return _elapsedTimeInMillis; }
		void reset(void) { _elapsedTimeInMillis = 0.0f; }
		void update(float updateTimeInMillis) { _elapsedTimeInMillis += updateTimeInMillis; }

	private:
		float _elapsedTimeInMillis;

		Timer(const Timer&);
		Timer& operator=(const Timer&);
	};
}