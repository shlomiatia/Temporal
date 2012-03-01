#pragma once

namespace Temporal { class Sensor; }

#include <Temporal\Base\Base.h>
#include "StaticBody.h"
#include "Sensor.h"
#include "StaticBodiesIndex.h"
#include <vector>

namespace Temporal
{
	class DynamicBody : public Body
	{
	public:
		static const float GRAVITY;

		DynamicBody(const Vector& size, const StaticBodiesIndex& staticBodiesIndex) : Body(size), _staticBodiesIndex(staticBodiesIndex), _movement(Vector::Zero), _gravityEnabled(true), _collision(Direction::NONE), _isImpulse(false) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::DYNAMIC_BODY; }
		Orientation::Enum getOrientation(void) const;

		void applyMovement(const Vector& movement) const;

		virtual void handleMessage(Message& message);
		void update(float framePeriodInMillis);

		void correctCollision(const StaticBody& staticBody, Vector& velocity);
		void detectCollision(const StaticBody& staticBody);

	private:
		Vector _movement;
		bool _isImpulse;
		bool _gravityEnabled;
		Direction::Enum _collision;
		const StaticBodiesIndex& _staticBodiesIndex;
	};
}