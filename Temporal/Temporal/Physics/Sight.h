#ifndef SIGHT_H
#define SIGHT_H

#include <Temporal\Base\BaseEnums.h>
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\Segment.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
	class Sight : public Component
	{
	public:
		Sight(float upperAngle, float lowerAngle) : _upperAngle(upperAngle), _lowerAngle(lowerAngle), _pointOfIntersection(Vector::Zero), _isSeeing(false) {};

		ComponentType::Enum getType(void) const { return ComponentType::SIGHT; }
		void handleMessage(Message& message);

	private:
		const float _upperAngle;
		const float _lowerAngle;

		Point _pointOfIntersection;
		bool _isSeeing;

		void checkLineOfSight(void);
		void drawFieldOfView(const Point& sourcePosition, Orientation::Enum sourceOrientation) const;
		void drawDebugInfo(void) const;

		bool directedSegmentCast(const DirectedSegment& ray);
	};
}
#endif