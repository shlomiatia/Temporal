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
		  _sightCenter(sightCenter), _sightSize(sightSize), _filter(NULL), _pointOfIntersection(Vector::Zero), _isSeeing(false) {};

		ComponentType::Enum getType() const { return ComponentType::SIGHT; }
		void handleMessage(Message& message);

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("center", _sightCenter);
			_sightCenter = toRadians(_sightCenter);
			serializer.serialize("size", _sightSize);
			_sightSize = toRadians(_sightSize);
		}
	private:
		const CollisionFilter* _filter;
		float _sightCenter;
		float _sightSize;
		
		Vector _pointOfIntersection;
		bool _isSeeing;

		void checkLineOfSight();
		void drawFieldOfView(const Vector& sourcePosition, Side::Enum sourceSide) const;
		void drawDebugInfo() const;
	};
}
#endif