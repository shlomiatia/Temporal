#ifndef DYNAMICBODY_H
#define DYNAMICBODY_H

#include <vector>
#include "NumericPair.h"
#include "Shapes.h"
#include "EntitySystem.h"

namespace Temporal
{
	class Segment;
	class StaticBody;

	class DynamicBody : public Component
	{
	public:
		// BRODER
		static const Vector GRAVITY;

		explicit DynamicBody(const Size& size);

		ComponentType::Enum getType() const { return ComponentType::DYNAMIC_BODY; }
		void handleMessage(Message& message);

	private:
		const float MAX_MOVEMENT_STEP_SIZE;
		const Size _size;
		
		// Persistent state
		bool _gravityEnabled;
		Vector _velocity;

		// Temp state
		Vector _absoluteImpulse;
		Vector _groundVector;

		AABB getBounds() const;
		Side::Enum getOrientation() const;

		void update(float framePeriodInMillis);
		Vector determineMovement(float framePeriodInMillis);
		void executeMovement(Vector movement);
		bool detectCollision(const StaticBody& staticBody, Vector& collision);
		void correctCollision(const AABB& dynamicBodyBounds, const Shape& staticBodyBounds, Vector& correction, Vector& collision);
		void modifyCorrection(const AABB& dynamicBodyBounds, const Segment& segment, Vector& correction, bool isModerateSlope);
		void modifyVelocity(const AABB& dynamicBodyBounds, const Segment& segment, const Vector& correction, const Vector& platformVector, bool isSteepSlope);
		void changePosition(const Vector& offset);

		static bool detectCollision(void* caller, void* data, const StaticBody& staticBody);
	};
}
#endif