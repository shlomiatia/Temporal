#ifndef LASER_H
#define LASER_H

#include "EntitySystem.h"
#include "Hash.h"

namespace Temporal
{
	static const float LASER_SPEED_PER_SECOND = 50.0f;

	class Laser : public Component
	{
	public:
		Laser(Hash platformID) 
			: _platformID(platformID), _isPositiveDirection(true) {}

		Hash getPlatformId() const { return _platformID; }
		
		ComponentType::Enum getType() const { return ComponentType::AI_CONTROLLER; }
		void handleMessage(Message& message);
	private:
		const Hash _platformID;

		bool _isPositiveDirection;

		void update(float framePeriodInMillis);
	};
}

#endif