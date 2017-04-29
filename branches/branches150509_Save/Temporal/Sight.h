#ifndef SIGHT_H
#define SIGHT_H

#include "Math.h"
#include "Hash.h"
#include "BaseEnums.h"
#include "Vector.h"
#include "EntitySystem.h"

namespace Temporal
{
	class CollisionFilter;

	class Sight : public Component
	{
	public:
		explicit Sight(float sightSize = 64.0f) :
		  _sightSize(sightSize), _filter(0), _pointOfIntersection(Vector::Zero), _isSeeing(false) {};

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new Sight(_sightSize); }

		static const Hash TYPE;
	private:
		const CollisionFilter* _filter;
		float _sightSize;
		
		Vector _pointOfIntersection;
		bool _isSeeing;

		void checkLineOfSight();
		void drawFieldOfView(const Vector& sourcePosition, Side::Enum sourceSide) const;
		void drawDebugInfo() const;

		friend class SerializationAccess;
	};
}
#endif