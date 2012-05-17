#ifndef MESSAGEPARAMS_H
#define MESSAGEPARAMS_H

#include "Hash.h"
#include "NumericPair.h"

namespace Temporal
{
	class ResetAnimationParams
	{
	public:
		explicit ResetAnimationParams(const Hash& animationID, bool rewind = false, bool repeat = false) : _animationID(animationID), _rewind(rewind), _repeat(repeat) {}

		const Hash& getAnimationID(void) const { return _animationID; }
		bool getRewind(void) const { return _rewind; }
		bool getRepeat(void) const { return _repeat; }

	private:
		const Hash _animationID;
		const bool _rewind;
		const bool _repeat;

		ResetAnimationParams(const ResetAnimationParams&);
		ResetAnimationParams& operator=(const ResetAnimationParams&);
	};

	class SensorCollisionParams
	{
	public:
		SensorCollisionParams(const Hash& sensorID, const Point* point) : _sensorID(sensorID), _point(point) {}

		const Hash& getSensorID(void) const { return _sensorID; }
		const Point* getPoint(void) const { return _point; }
	
	private:
		const Hash _sensorID;
		const Point* _point;
	
	};
}
#endif