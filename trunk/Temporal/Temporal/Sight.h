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
		explicit Sight(Vector sightSize = Vector(480.0f, 64.0f)) :
		  _sightSize(sightSize) {};

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new Sight(_sightSize); }

		static const Hash TYPE;
	private:
		Vector _sightSize;
		
		void checkLineOfSight();
		void drawFieldOfView(const Vector& sourcePosition, Side::Enum sourceSide) const;
		void drawDebugInfo() const;

		friend class SerializationAccess;
	};
}
#endif