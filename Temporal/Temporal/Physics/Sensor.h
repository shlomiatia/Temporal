#pragma once

namespace Temporal { class DynamicBody; }

#include <Temporal\Base\Base.h>
#include "DynamicBody.h"

namespace Temporal
{
	class Sensor : public Component
	{
	public:
		Sensor(SensorID::Enum id, const Vector& offset, const Vector& size, Direction::Enum positive, Direction::Enum negative)
			: _id(id), _offset(offset), _size(size), _sensedBody(NULL), _positive(positive), _negative(negative) {}

		SensorID::Enum getID() const { return _id; }
		const Vector& getSize(void) const { return _size; }
		Rect getBounds(void) const;
		const Body* const getSensedBody(void) const { return _sensedBody; }
		
		virtual void handleMessage(Message& message);
		virtual ComponentType::Enum getType(void) const { return ComponentType::SENSOR; }

	private:
		const SensorID::Enum _id;
		const Vector _offset;
		const Vector _size;
		Direction::Enum _positive;
		Direction::Enum _negative;
		const Body* _sensedBody;

		void update(void);
		bool sense(const StaticBody &staticBody);
		static bool sense(void* caller, void* data, const StaticBody& staticBody);
	};
}
