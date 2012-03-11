#include "Utils.h"
#include "StaticBody.h"
#include <Temporal\Game\EntitiesManager.h>

namespace Temporal
{
	Direction::Enum calculateCollision(const Rect& boundsA, Orientation::Enum orientationA, const Rect& boundsB)
	{
		Direction::Enum collision(Direction::NONE);

		if(boundsA.intersectsInclusive(boundsB))
		{
			float frontA = boundsA.getSide(orientationA);
			float frontB = boundsB.getSide(orientationA);
			float backA = boundsA.getOppositeSide(orientationA);
			float backB = boundsB.getOppositeSide(orientationA);
			bool overlapsX = boundsA.getLeft() < boundsB.getRight() && boundsA.getRight() > boundsB.getLeft();
			bool overlapsY = boundsA.getBottom() < boundsB.getTop() && boundsA.getTop() > boundsB.getBottom();
			if(boundsA.getTop() >= boundsB.getTop() && overlapsX) collision = collision | Direction::BOTTOM;
			if(boundsA.getBottom() <= boundsB.getBottom() && overlapsX) collision = collision | Direction::TOP;
			if((frontA - frontB) * orientationA >= 0 && overlapsY) collision = collision | Direction::BACK;
			if((backB - backA) * orientationA >= 0 && overlapsY) collision = collision | Direction::FRONT;
		}

		return collision;
	}
}