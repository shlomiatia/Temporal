#ifndef DYNAMICBODY_H
#define DYNAMICBODY_H

#include <vector>
#include "Vector.h"
#include "EntitySystem.h"

namespace Temporal
{
	class Shape;
	class Segment;
	class Fixture;

	class DynamicBody : public Component
	{
	public:
		// BRODER
		static const Vector GRAVITY;

		explicit DynamicBody(Fixture* fixture = NULL) : _fixture(fixture), _velocity(Vector::Zero), _absoluteImpulse(Vector::Zero), _gravityEnabled(true),
			_groundVector(Vector::Zero), _maxMovementStepSize(0.0f) {}

		~DynamicBody();

		ComponentType::Enum getType() const { return ComponentType::DYNAMIC_BODY; }
		void handleMessage(Message& message);

		Component* clone() const;

	private:
		float _maxMovementStepSize;
		Fixture* _fixture;
		
		// Persistent state
		bool _gravityEnabled;
		Vector _velocity;

		// Temp state
		Vector _absoluteImpulse;
		Vector _groundVector;

		void update(float framePeriod);
		Vector determineMovement(float framePeriod);
		void executeMovement(Vector movement);
		void detectCollision(Shape& dynamicBodyBounds, const Shape& staticBodyBounds, Vector& collision);
		void correctCollision(Shape& dynamicBodyBounds, const Shape& staticBodyBounds, Vector& correction, Vector& collision);
		void modifyCorrection(const Shape& dynamicBodyBounds, const Segment& segment, Vector& correction, bool isModerateSlope);
		void modifyVelocity(const Shape& dynamicBodyBounds, const Segment& segment, const Vector& correction, const Vector& platformVector, bool isSteepSlope);

		friend class SerializationAccess;
	};
}
#endif