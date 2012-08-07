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

		explicit DynamicBody(const CollisionInfo* collisionInfo);

		ComponentType::Enum getType() const { return ComponentType::DYNAMIC_BODY; }
		void handleMessage(Message& message);

	private:
		const float MAX_MOVEMENT_STEP_SIZE;
		const CollisionInfo* _collisionInfo;
		
		// Persistent state
		bool _gravityEnabled;
		Vector _velocity;

		// Temp state
		Vector _absoluteImpulse;
		Vector _groundVector;

		Side::Enum getOrientation() const;

		void update(float framePeriodInMillis);
		Vector determineMovement(float framePeriodInMillis);
		void executeMovement(Vector movement);
		void detectCollision(const CollisionInfo& info, Vector& collision);
		void correctCollision(const Shape& dynamicBodyBounds, const Shape& staticBodyBounds, Vector& correction, Vector& collision);
		void modifyCorrection(const Shape& dynamicBodyBounds, const Segment& segment, Vector& correction, bool isModerateSlope);
		void modifyVelocity(const Shape& dynamicBodyBounds, const Segment& segment, const Vector& correction, const Vector& platformVector, bool isSteepSlope);
		void changePosition(const Vector& offset);
	};
}
#endif