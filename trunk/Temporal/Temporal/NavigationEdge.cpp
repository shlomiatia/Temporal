#include "NavigationEdge.h"
#include "NavigationNode.h"
#include "Shapes.h"

namespace Temporal
{
	float NavigationEdge::calculateCost(const NavigationNode& source)
	{
		const OBB& sourceArea = source.getArea();
		const OBB& targetArea = getTarget().getArea();

		Segment segment(sourceArea.getCenter(), targetArea.getCenter());
		return segment.getLength();
	}
}