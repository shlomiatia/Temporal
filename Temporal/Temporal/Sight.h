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
		explicit Sight(float sightCenter = 0.0f, float sightSize = 0.0f) :
		  _sightCenter(sightCenter), _sightSize(sightSize), _filter(0), _pointOfIntersection(Vector::Zero), _isSeeing(false) {};

		ComponentType::Enum getType() const { return ComponentType::SIGHT; }
		void handleMessage(Message& message);

		Component* clone() const { return new Sight(_sightCenter, _sightSize); }

	private:
		const CollisionFilter* _filter;
		float _sightCenter;
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