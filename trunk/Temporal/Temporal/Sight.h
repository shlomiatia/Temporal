#ifndef SIGHT_H
#define SIGHT_H

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
		Sight(float sightCenter, float sightSize, const CollisionFilter& collisionFilter) :
		  _sightCenter(sightCenter), _sightSize(sightSize), _pointOfIntersection(Vector::Zero), _isSeeing(false), _collisionFilter(collisionFilter) {};

		ComponentType::Enum getType() const { return ComponentType::SIGHT; }
		void handleMessage(Message& message);

	private:
		const float _sightCenter;
		const float _sightSize;
		const CollisionFilter& _collisionFilter;

		Vector _pointOfIntersection;
		bool _isSeeing;

		void checkLineOfSight();
		void drawFieldOfView(const Vector& sourcePosition, Side::Enum sourceSide) const;
		void drawDebugInfo() const;
	};
}
#endif