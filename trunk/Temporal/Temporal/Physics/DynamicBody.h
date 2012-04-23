#ifndef DYNAMICBODY_H
#define DYNAMICBODY_H

#include <vector>
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\AABB.h>
#include <Temporal\Game\Component.h>

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

		ComponentType::Enum getType(void) const { return ComponentType::DYNAMIC_BODY; }

		void handleMessage(Message& message);

	private:
		const float MAX_MOVEMENT_STEP_SIZE;
		
		Size _size;
		Vector _velocity;
		Vector _absoluteImpulse;
		bool _gravityEnabled;
		Vector _collision;
		Vector _groundVector;

		AABB getBounds(void) const;
		Orientation::Enum getOrientation(void) const;

		void update(float framePeriodInMillis);
		Vector determineMovement(float framePeriodInMillis);
		void executeMovement(Vector movement);
		bool detectCollision(const StaticBody& staticBody);
		void correctCollision(const AABB& dynamicBodyBounds, const Shape& staticBodyBounds, Vector& correction);
		void modifyCorrection(const AABB& dynamicBodyBounds, const Segment& segment, Vector& correction, bool isModerateSlope);
		void modifyVelocity(const AABB& dynamicBodyBounds, const Segment& segment, const Vector& correction, const Vector& platformVector, bool isSteepSlope);
		void changePosition(const Vector& offset);

		static bool detectCollision(void* caller, void* data, const StaticBody& staticBody);
	};
}
#endif