#pragma once

namespace Temporal { class Sensor; }

#include <Temporal\Base\Base.h>
#include "StaticBody.h"
#include "Sensor.h"
#include <vector>

namespace Temporal
{
	class DynamicBody : public Body
	{
	public:
		static const float GRAVITY_PER_SECOND;
		static const float MAX_GRAVITY_PER_SECOND;

		DynamicBody(const Vector& size) : Body(size), _velocity(Vector::Zero), _gravityEnabled(true), _collision(Direction::NONE) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::DYNAMIC_BODY; }
		Orientation::Enum getOrientation(void) const;

		void applyGravity(float interpolation);
		void applyMovement(const Vector& movement) const;

		virtual void handleMessage(Message& message);
		void update(float framePeriodInMillis);
		bool rayCast(const Vector& destination) const;

		void correctCollision(const StaticBody& staticBody);
		void detectCollision(const StaticBody& staticBody);

	private:
		Vector _velocity;
		bool _gravityEnabled;
		Direction::Enum _collision;
	};
}