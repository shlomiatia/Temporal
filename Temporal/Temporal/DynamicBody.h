#ifndef DYNAMICBODY_H
#define DYNAMICBODY_H

#include <vector>
#include "Vector.h"
#include "EntitySystem.h"

namespace Temporal
{
	class YABP;
	class Fixture;

	class DynamicBody : public Component
	{
	public:
		static Vector GRAVITY;

		explicit DynamicBody(Fixture* fixture = 0) :
			_fixture(fixture), _velocity(Vector::Zero), _gravityEnabled(true), _ground(0), _maxMovementStepSize(0.0f),
			_previousGroundCenter(Vector::Zero) {}

		~DynamicBody();

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const;

		static const Hash TYPE;
	private:

		float _maxMovementStepSize;
		Fixture* _fixture;
		
		// Persistent state
		bool _gravityEnabled;
		Vector _velocity;

		// Temp state
		const Fixture* _ground;
		Vector _previousGroundCenter;

		void walk(float framePeriod);
		void update(float framePeriod);
		Vector determineMovement(float framePeriod);
		void executeMovement(Vector movement);
		void detectCollision(YABP& dynamicBodyBounds, const Fixture* staticBodyBounds, Vector& collision, Vector& movement);
		void correctCollision(YABP& dynamicBodyBounds, const Fixture* staticBodyBounds, Vector& correction, Vector& collision, Vector& movement);
		void modifyCorrection(const YABP& dynamicBodyBounds, const Fixture* staticBodyBounds, Vector& correction, Vector& movement);
		void modifyVelocity(const Vector& correction);

		friend class SerializationAccess;
	};
}
#endif