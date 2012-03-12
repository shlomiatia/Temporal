#pragma once

#include "Body.h"
#include <vector>

namespace Temporal
{
	class StaticBody;

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
		void applyVelocity() const;
		void determineVelocity(float framePeriodInMillis);
		void handleCollisions(void);
		void correctCollision(const StaticBody& staticBody);
		void detectCollision(const StaticBody& staticBody);
		static bool correctCollision(void* caller, void* data, const StaticBody& staticBody);
		static bool detectCollision(void* caller, void* data, const StaticBody& staticBody);
	};
}