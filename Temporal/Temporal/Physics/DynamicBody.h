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
		static const float GRAVITY;
		static const float MAX_GRAVITY;

		DynamicBody(const Vector& size) : Body(size), _force(Vector::Zero), _gravityEnabled(true), _collision(Direction::NONE) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::DYNAMIC_BODY; }
		Orientation::Enum getOrientation(void) const;

		void applyForce(void);
		void applyGravity(void);

		virtual void handleMessage(Message& message);
		void update(void);
		bool rayCast(const Vector& destination) const;

		void correctCollision(const StaticBody& staticBody);
		void detectCollision(const StaticBody& staticBody);

	private:
		Vector _force;
		bool _gravityEnabled;
		Direction::Enum _collision;
	};
}