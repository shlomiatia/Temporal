#ifndef SENSOR_H
#define SENSOR_H

#include <Temporal\Base\BaseEnums.h>
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\AABB.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
	class StaticBody;

	class Sensor : public Component
	{
	public:
		Sensor(SensorID::Enum id, const Vector& offset, const Size& size, float rangeCenter, float rangeSize)
			: _id(id), _offset(offset), _size(size), _point(Point::Zero), _rangeCenter(rangeCenter), _rangeSize(rangeSize) {}

		SensorID::Enum getID() const { return _id; }
		const Point* getPoint(void) const { return _point == Point::Zero ? NULL : &_point; }
		
		void handleMessage(Message& message);
		ComponentType::Enum getType(void) const { return ComponentType::SENSOR; }

	private:
		const SensorID::Enum _id;
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