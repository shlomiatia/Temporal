#pragma once

namespace Temporal { class Sensor; }

#include <Temporal\Base\Base.h>
#include "StaticBody.h"
#include "Sensor.h"
#include "StaticBodiesIndex.h"
#include <vector>

// TODO: Fix 2 jump bugs in physics (stuck in platform & float while jumping beneath platform) SLOTH!
namespace Temporal
{
	class DynamicBody : public Body
	{
	public:
		static const float GRAVITY;

		DynamicBody(const Vector& size) : Body(size), _velocity(Vector::Zero), _force(Vector::Zero), _impulse(Vector::Zero), _gravityEnabled(true), _collision(Direction::NONE) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::DYNAMIC_BODY; }
		Orientation::Enum getOrientation(void) const;

		virtual void handleMessage(Message& message);

	private:
		Vector _velocity;
		Vector _force;
		Vector _impulse;
		bool _gravityEnabled;
		Direction::Enum _collision;

		void update(float framePeriodInMillis);
		void applyMovement(const Vector& movement) const;
		void determineVelocity(float framePeriodInMillis);
		void handleCollisions(void);
		void correctCollision(const StaticBody& staticBody);
		void detectCollision(const StaticBody& staticBody);
		static void correctCollision(void* caller, void* data, const StaticBody& staticBody);
		static void detectCollision(void* caller, void* data, const StaticBody& staticBody);
	};
}