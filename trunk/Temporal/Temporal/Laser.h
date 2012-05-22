#ifndef LASER_H
#define LASER_H

#include "Component.h"
#include "Hash.h"
#include "NumericPair.h"

namespace Temporal
{
	static const float LASER_SPEED_PER_SECOND = 50.0f;

	class Laser : public Component
	{
	public:
		Laser(const Hash& platformID) : _platformID(platformID), _isPositiveDirection(true), _pointOfIntersection(Point::Zero), _isDetecting(false) {}

		ComponentType::Enum getType(void) const { return ComponentType::AI_CONTROLLER; }
		void handleMessage(Message& message);
	private:
		const Hash _platformID;
		bool _isPositiveDirection;
		Point _pointOfIntersection;
		bool _isDetecting;

		void update(float framePeriodInMillis);
	};
}

#endif