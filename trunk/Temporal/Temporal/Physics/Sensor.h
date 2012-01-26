#pragma once

namespace Temporal { class DynamicBody; }

#include <Temporal\Base\Base.h>
#include "DynamicBody.h"

namespace Temporal
{
	class Sensor
	{
	public:
		Rect getBounds(void) const;

		const DynamicBody& getOwner(void) const { return _owner; }
		const Body* getSensedBody(void) const { return _sensedBody; }
		Direction::Type getSensedBodyDirection(void) const { return _sensedBodyDirection; }
		void clearCollision(void) { _sensedBody = NULL; _sensedBodyDirection = Direction::NONE; }
		void setCollision(const Body* sensedBody, Direction::Type sensedBodyDirection) {  _sensedBody = sensedBody; _sensedBodyDirection = sensedBodyDirection; }

		Sensor(DynamicBody& owner, const Vector& offsetFromBody, const Vector& size);
		~Sensor(void) {}
	private:
		
		const DynamicBody& _owner;

		const Vector _offsetFromBody;
		const Vector _size;

		const Body* _sensedBody;
		Direction::Type _sensedBodyDirection;

		Sensor(const Sensor&);
		Sensor& operator=(const Sensor&);
	};
}
