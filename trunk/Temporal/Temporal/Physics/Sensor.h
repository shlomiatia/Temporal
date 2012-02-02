#pragma once

namespace Temporal { class DynamicBody; }

#include <Temporal\Base\Base.h>
#include "DynamicBody.h"

namespace Temporal
{
	class Sensor
	{
	public:
		Sensor(SensorID::Type id, DynamicBody& owner, const Vector& offsetFromBody, const Vector& size, Direction::Type positive, Direction::Type negative);
		~Sensor(void) {}

		const DynamicBody& getOwner(void) const { return _owner; }
		SensorID::Type getID() const { return _id; }
		Rect getBounds(void) const;
		const Body* const getSensedBody(void) const { return _sensedBody; }
		void setSensedBody(const Body* const sensedBody) { _sensedBody = sensedBody; }
		Direction::Type getPositive(void) const { return _positive; }
		Direction::Type getNegative(void) const { return _negative; }

	private:
		const DynamicBody& _owner;

		const SensorID::Type _id;
		const Vector _offsetFromBody;
		const Vector _size;

		const Body* _sensedBody;
		Direction::Type _positive;
		Direction::Type _negative;

		Sensor(const Sensor&);
		Sensor& operator=(const Sensor&);
	};
}
