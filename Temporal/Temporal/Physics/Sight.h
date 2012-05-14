#ifndef SIGHT_H
#define SIGHT_H

#include <Temporal\Base\Hash.h>
#include <Temporal\Base\BaseEnums.h>
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\Segment.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
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