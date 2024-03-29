#ifndef DYNAMICBODY_H
#define DYNAMICBODY_H

#include "Vector.h"
#include "EntitySystem.h"
#include "Shapes.h"
#include "Ids.h"

namespace Temporal
{
	class Fixture;

	class DynamicBody : public Component
	{
	public:
		static Vector GRAVITY;

		explicit DynamicBody(Fixture* fixture = 0, bool gravityEnabled = true, int collisionMask = 0) :
			_fixture(fixture), _velocity(Vector::Zero), _gravityEnabled(gravityEnabled), _ground(0), _maxMovementStepSize(0.0f),
			_previousGroundCenter(Vector::Zero), _collisionMask(collisionMask), _groundId(Hash::INVALID){}

		~DynamicBody();

		Hash getType() const { return ComponentsIds::DYNAMIC_BODY; }
		void handleMessage(Message& message);
		Component* clone() const;

		Fixture& getFixture() { return *_fixture; }

		
	private:
		float _maxMovementStepSize;
		Fixture* _fixture;
		int _collisionMask;
		
		// Persistent state
		bool _gravityEnabled;
		Vector _velocity;

		// Temp state
		Hash _groundId;
		const Fixture* _ground;
		Vector _previousGroundCenter;

		void update(float framePeriod);
		Vector determineMovement(float framePeriod);
		void handleSlide(Segment groundSegment, bool isModerateGround);
		void setGround(const Fixture* ground);
		void handleMovingPlatform(OBBAABBWrapper dynamicBodyBounds);
		void resetGround();
		void walk(OBBAABBWrapper& dynamicBodyBounds, float framePeriod);
		void executeMovement(OBBAABBWrapper& dynamicBodyBounds, Vector movement);
		void detectCollision(OBBAABBWrapper& dynamicBodyBounds, const Fixture* staticBodyBounds, Vector& collision, Vector& movement);
		void correctCollision(OBBAABBWrapper& dynamicBodyBounds, const Fixture* staticBodyBounds, Vector& correction, Vector& collision, Vector& movement);
		void modifyCorrection(OBBAABBWrapper& dynamicBodyBounds, const Fixture* staticBodyBounds, Vector& correction, Vector& movement);
		void modifyVelocity(const Vector& collision);
		const Fixture* transitionPlatform(OBBAABBWrapper& dynamicBodyBounds, const Vector& direction, Side::Enum side, float leftPeriod);
		void loseGround();

		friend class SerializationAccess;
	};
}
#endif