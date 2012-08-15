#ifndef DYNAMICBODY_H
#define DYNAMICBODY_H

#include <vector>
#include "NumericPair.h"
#include "EntitySystem.h"

namespace Temporal
{
	class Shape;
	class Segment;
	class CollisionInfo;

	class DynamicBody : public Component
	{
	public:
		// BRODER
		static const Vector GRAVITY;

		explicit DynamicBody(CollisionInfo* collisionInfo);

		ComponentType::Enum getType() const { return ComponentType::DYNAMIC_BODY; }
		void handleMessage(Message& message);

	private:
		const float MAX_MOVEMENT_STEP_SIZE;
		CollisionInfo* _collisionInfo;
		
		// Persistent state
		bool _gravityEnabled;
		Vector _velocity;

		// Temp state
		Vector _absoluteImpulse;
		Vector _groundVector;

		void update(float framePeriodInMillis);
		Vector determineMovement(float framePeriodInMillis);
		void executeMovement(Vector movement);
		void detectCollision(Shape& dynamicBodyBounds, const Shape& staticBodyBounds, Vector& collision);
		void correctCollision(Shape& dynamicBodyBounds, const Shape& staticBodyBounds, Vector& correction, Vector& collision);
		void modifyCorrection(const Shape& dynamicBodyBounds, const Segment& segment, Vector& correction, bool isModerateSlope);
		void modifyVelocity(const Shape& dynamicBodyBounds, const Segment& segment, const Vector& correction, const Vector& platformVector, bool isSteepSlope);
	};
}
#endif