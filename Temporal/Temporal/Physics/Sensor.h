#pragma once

namespace Temporal { class Body; }

#include <Temporal\Base\Base.h>
#include "Body.h"

namespace Temporal
{
	class Sensor
	{
	public:
		Rect getBounds(void) const;

		const Body& getOwner(void) const { return _owner; }
		const Body* getSensedBody(void) const { return _sensedBody; }
		Direction::Type getSensedBodyDirection(void) const { return _sensedBodyDirection; }
		void clearCollision(void) { _sensedBody = NULL; _sensedBodyDirection = Direction::NONE; }
		void setCollision(const Body* sensedBody, Direction::Type sensedBodyDirection) {  _sensedBody = sensedBody; _sensedBodyDirection = sensedBodyDirection; }

		Sensor(Body& owner, const Vector& offsetFromBody, const Vector& size);
		~Sensor(void) {}
	private:
		
		const Body& _owner;

		const Vector _offsetFromBody;
		const Vector _size;

		const Body* _sensedBody;
		Direction::Type _sensedBodyDirection;

		Sensor(const Sensor&);
		Sensor& operator=(const Sensor&);
	};
}
