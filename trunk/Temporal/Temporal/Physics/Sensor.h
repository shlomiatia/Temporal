#pragma once

namespace Temporal { class DynamicBody; }

#include <Temporal\Base\Base.h>
#include "DynamicBody.h"

namespace Temporal
{
	class Sensor
	{
	public:
		Sensor(SensorID::Enum id, DynamicBody& owner, const Vector& offsetFromBody, const Vector& size, Direction::Enum positive, Direction::Enum negative);
		~Sensor(void) {}

		const DynamicBody& getOwner(void) const { return _owner; }
		SensorID::Enum getID() const { return _id; }
		Rect getBounds(void) const;
		const Body* const getSensedBody(void) const { return _sensedBody; }
		void setSensedBody(const Body* const sensedBody) { _sensedBody = sensedBody; }
		Direction::Enum getPositive(void) const { return _positive; }
		Direction::Enum getNegative(void) const { return _negative; }

	private:
		const DynamicBody& _owner;

		const SensorID::Enum _id;
		const Vector _offsetFromBody;
		const Vector _size;

		const Body* _sensedBody;
		Direction::Enum _positive;
		Direction::Enum _negative;

		Sensor(const Sensor&);
		Sensor& operator=(const Sensor&);
	};
}
