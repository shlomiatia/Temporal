#ifndef SENSOR_H
#define SENSOR_H

#include "Hash.h"
#include "BaseEnums.h"
#include "NumericPair.h"
#include "EntitySystem.h"

namespace Temporal
{
	class CollisionInfo;

	class Sensor : public Component
	{
	public:
		Sensor(const Hash& id, const CollisionInfo* collisionInfo, float rangeCenter, float rangeSize)
			: _id(id), _collisionInfo(collisionInfo), _point(Point::Zero), _rangeCenter(rangeCenter), _rangeSize(rangeSize) {}
		
		void handleMessage(Message& message);
		ComponentType::Enum getType() const { return ComponentType::SENSOR; }

	private:
		const Hash _id;
		const CollisionInfo* _collisionInfo;
		const float _rangeCenter;
		const float _rangeSize;
		Point _point;

		void update();
	};
}

#endif