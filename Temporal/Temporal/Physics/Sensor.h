#ifndef SENSOR_H
#define SENSOR_H

#include <Temporal\Base\BaseEnums.h>
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\Rect.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
	class StaticBody;

	class Sensor : public Component
	{
	public:
		Sensor(SensorID::Enum id, const Vector& offset, const Size& size, Direction::Enum positive, Direction::Enum negative)
			: _id(id), _offset(offset), _size(size), _sensedBody(NULL), _positive(positive), _negative(negative) {}

		SensorID::Enum getID() const { return _id; }
		const StaticBody* getSensedBody(void) const { return _sensedBody; }
		
		void handleMessage(Message& message);
		ComponentType::Enum getType(void) const { return ComponentType::SENSOR; }

	private:
		const SensorID::Enum _id;
		const Vector _offset;
		const Size _size;
		const Direction::Enum _positive;
		const Direction::Enum _negative;
		const StaticBody* _sensedBody;

		Rect getBounds(void) const;
		void update(void);
		bool sense(const StaticBody &staticBody);
		static bool sense(void* caller, void* data, const StaticBody& staticBody);
	};
}

#endif