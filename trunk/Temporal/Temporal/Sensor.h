#ifndef SENSOR_H
#define SENSOR_H

#include "Hash.h"
#include "BaseEnums.h"
#include "NumericPair.h"
#include "Shapes.h"
#include "EntitySystem.h"

namespace Temporal
{
	class StaticBody;

	class Sensor : public Component
	{
	public:
		Sensor(const Hash& id, const AABB& bounds, float rangeCenter, float rangeSize)
			: _id(id), _bounds(bounds), _point(Point::Zero), _rangeCenter(rangeCenter), _rangeSize(rangeSize) {}
		
		void handleMessage(Message& message);
		ComponentType::Enum getType() const { return ComponentType::SENSOR; }

	private:
		const Hash _id;
		const AABB _bounds;
		const float _rangeCenter;
		const float _rangeSize;
		Point _point;

		AABB getBounds() const;
		void update();
		bool sense(const StaticBody &staticBody);
		static bool sense(void* caller, void* data, const StaticBody& staticBody);
	};
}

#endif