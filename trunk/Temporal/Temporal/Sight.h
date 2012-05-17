#ifndef SIGHT_H
#define SIGHT_H

#include "Hash.h"
#include "BaseEnums.h"
#include "NumericPair.h"
#include "Component.h"

namespace Temporal
{
	class DirectedSegment;

	class Sight : public Component
	{
	public:
		Sight(float sightCenter, float sightSize, const Hash& targetID) :
		  _sightCenter(sightCenter), _sightSize(sightSize), _pointOfIntersection(Vector::Zero), _isSeeing(false), _targetID(targetID) {};

		ComponentType::Enum getType(void) const { return ComponentType::SIGHT; }
		void handleMessage(Message& message);

	private:
		const float _sightCenter;
		const float _sightSize;

		Hash _targetID;

		Point _pointOfIntersection;
		bool _isSeeing;

		void checkLineOfSight(void);
		void drawFieldOfView(const Point& sourcePosition, Side::Enum sourceSide) const;
		void drawDebugInfo(void) const;

		bool directedSegmentCast(const DirectedSegment& ray);
	};
}
#endif