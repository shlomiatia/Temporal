#ifndef DYNAMICBODY_H
#define DYNAMICBODY_H

#include <vector>
#include "Vector.h"
#include "EntitySystem.h"
#include "Shapes.h"

namespace Temporal
{
	class Fixture;

	class DynamicBody : public Component
	{
	public:
		static Vector GRAVITY;

		explicit DynamicBody(Fixture* fixture = 0, bool gravityEnabled = true, int collisionMask = 0) :
			_fixture(fixture), _velocity(Vector::Zero), _bodyEnabled(true), _gravityEnabled(gravityEnabled), _ground(0), _maxMovementStepSize(0.0f),
			_previousGroundCenter(Vector::Zero), _dynamicBodyBounds(&_globalShapeClone), _collisionMask(collisionMask), _groundId(Hash::INVALID){}

		~DynamicBody();

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const;

		static const Hash TYPE;
	private:

		float _maxMovementStepSize;
		Fixture* _fixture;
		OBB _globalShapeClone;
		OBBAABBWrapper _dynamicBodyBounds;
		int _collisionMask;
		
		// Persistent state
		bool _gravityEnabled;
		bool _bodyEnabled;
		Vector _velocity;

		// Temp state
		Hash _groundId;
		const Fixture* _ground;
		//Segment _groundSegment;
		Vector _previousGroundCenter;

		void update(float framePeriod);
		Vector determineMovement(float framePeriod);
		void walk(float framePeriod, const Segment& groundSegment);
		void executeMovement(Vector movement);
		void detectCollision(const Fixture* staticBodyBounds, Vector& collision, Vector& movement);
		void correctCollision(const Fixture* staticBodyBounds, Vector& correction, Vector& collision, Vector& movement);
		void modifyCorrection(const Fixture* staticBodyBounds, Vector& correction, Vector& movement);
		void modifyVelocity(const Vector& collision);
		bool transitionPlatform(const Vector& direction, Side::Enum side, float leftPeriod);
		void resetGround();

		friend class SerializationAccess;
	};
}
#endif