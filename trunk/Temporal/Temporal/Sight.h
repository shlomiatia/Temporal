#ifndef SIGHT_H
#define SIGHT_H

#include "Math.h"
#include "Hash.h"
#include "BaseEnums.h"
#include "Vector.h"
#include "EntitySystem.h"

namespace Temporal
{
	class Sight : public Component
	{
	public:
		explicit Sight(Vector sightOffset = Vector::Zero, Vector sightSize = Vector(480.0f, 32.0f)) : _sightOffset(sightOffset), _sightSize(sightSize) {};

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new Sight(_sightOffset, _sightSize); }

		static const Hash TYPE;
	private:
		Vector _sightOffset;
		Vector _sightSize;
		
		void checkLineOfSight(int collisionMask);
		void drawFieldOfView(const Vector& sourcePosition, Side::Enum sourceSide) const;
		void drawDebugInfo() const;

		friend class SerializationAccess;
	};
}
#endif