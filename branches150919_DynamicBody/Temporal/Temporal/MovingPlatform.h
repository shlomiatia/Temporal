#ifndef MOVINGPLATFORM_H
#define MOVINGPLATFORM_H

#include "Vector.h"
#include "EntitySystem.h"

namespace Temporal
{
	class MovingPlatform : public Component
	{
	public:
		MovingPlatform(const Vector& movement = Vector(128.0f, 0)) : _movement(movement) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new MovingPlatform(_movement); }
		const Vector& getMovement() const { return _movement; }
		Vector& getMovement() { return _movement; }

		static const Hash TYPE;
	private:
		Vector _movement;

		friend class SerializationAccess;
	};
}

#endif