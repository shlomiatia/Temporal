#ifndef SENSOR_H
#define SENSOR_H

#include "Hash.h"
#include "BaseEnums.h"
#include "NumericPair.h"
#include "AABB.h"
#include "Component.h"

namespace Temporal
{
	class StaticBody;

	class Sensor : public Component
	{
	public:
		Sensor(const Hash& id, const Vector& offset, const Size& size, float rangeCenter, float rangeSize)
			: _id(id), _offset(offset), _size(size), _point(Point::Zero), _rangeCenter(rangeCenter), _rangeSize(rangeSize) {}
		
		void handleMessage(Message& message);
		ComponentType::Enum getType(void) const { return ComponentType::SENSOR; }

	private:
		const Hash _id;
		const Vector _offset;
		const Size _size;
		const float _rangeCenter;
		const float _rangeSize;
		Point _point;

		AABB getBounds(void) const;
		void update(void);
		bool sense(const StaticBody &staticBody);
		static bool sense(void* caller, void* data, const StaticBody& staticBody);
	};
}

#endif