#include "Sight.h"
#include "Grid.h"
#include "StaticBody.h"
#include <Temporal\Base\Math.h>
#include <Temporal\Game\Message.h>
#include <Temporal\Game\EntitiesManager.h>
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	void Sight::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			checkLineOfSight();
		}
		else if(message.getID() == MessageID::DEBUG_DRAW)
		{
			drawDebugInfo();
		}
	}

	void Sight::checkLineOfSight(void)
	{
		_pointOfIntersection = Point::Zero;
		_isSeeing = false;
		const Point& sourcePosition = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Orientation::Enum sourceOrientation = *(Orientation::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		const Point& targetPosition = *(Point*)EntitiesManager::get().sendMessageToEntity(0, Message(MessageID::GET_POSITION));

		// Check orientation
		if((targetPosition.getX() - sourcePosition.getX()) * sourceOrientation < 0.0f)
			return;

		// Check field of view
		// TODO: Broder
		DirectedSegment directedSegment(sourcePosition, targetPosition);
		float angle = directedSegment.getAngle();
		if(angle < 0.0f) angle = 2*PI + angle;
		float sightCenter = sourceOrientation == Orientation::RIGHT ? _sightCenter : PI - _sightCenter;
		float distance = std::min(abs(sightCenter - angle),  abs(angle - 2*PI - sightCenter));
		if(distance > _sightSize / 2.0f) return;
		
		_isSeeing = directedSegmentCast(directedSegment);
		
		if(_isSeeing)
			sendMessageToOwner(Message(MessageID::LINE_OF_SIGHT));
	}

	bool Sight::directedSegmentCast(const DirectedSegment& directedSegment)
	{
		const Point& source = directedSegment.getOrigin();
		const Point& destination = directedSegment.getTarget();
		float x1 = source.getX();
		float y1 = source.getY();
		float x2 = destination.getX();
		float y2 = destination.getY();
		
		int i = Grid::get().getAxisIndex(x1);
		int j = Grid::get().getAxisIndex(y1);

		// Determine end grid cell coordinates (iend, jend)
		int iend = Grid::get().getAxisIndex(x2);
		int jend = Grid::get().getAxisIndex(y2);

		// Determine in which primary direction to step
		int di = ((x1 < x2) ? 1 : ((x1 > x2) ? -1 : 0));
		int dj = ((y1 < y2) ? 1 : ((y1 > y2) ? -1 : 0));

		const float tileSize = Grid::get().getTileSize();

		// Determine tx and ty, the values of t at which the directed segment
		// (x1,y1)-(x2,y2) crosses the first horizontal and vertical cell
		// boundaries, respectively. Min(tx, ty) indicates how far one can
		// travel along the segment and still remain in the current cell
		float minx = tileSize * floorf(x1/tileSize), maxx = minx + tileSize;
		float tx = ((x1 > x2) ? (x1 - minx) : (maxx - x1)) / abs(x2 - x1);
		float miny = tileSize * floorf(y1/tileSize), maxy = miny + tileSize;
		float ty = ((y1 > y2) ? (y1 - miny) : (maxy - y1)) / abs(y2 - y1);

		// Determine deltax/deltay, how far (in units of t) one must step
		// along the directed line segment for the horizontal/vertical
		// movement (respectively) to equal the width/height of a cell
		float deltatx = tileSize / abs(x2 - x1);
		float deltaty = tileSize / abs(y2 - y1);

		_pointOfIntersection = destination;
		// Main loop. Visits cells until last cell reached
		while(true)
		{
			StaticBodyCollection* staticBodies = Grid::get().getTile(i, j);
			if(staticBodies != NULL)
			{
				for(StaticBodyIterator iterator = staticBodies->begin(); iterator != staticBodies->end(); ++iterator)
				{
					const StaticBody& body = **iterator;
					if(body.getSegment().intersects(directedSegment, _pointOfIntersection))
					{
						return false;
					}
				}
				
			}
			if (tx <= ty) 
			{ // tx smallest, step in x
				if (i == iend) break;
				tx += deltatx;
				i += di;
			}
			else
			{ // ty smallest, step in y
				if (j == jend) break;
				ty += deltaty;
				j += dj;
			}
		}
			
		return true;
	}

	void drawFieldOfViewSegment(float angle, Orientation::Enum sourceOrientation, const Point &sourcePosition)
	{
		if(sourceOrientation == Orientation::LEFT)
			angle = PI - angle;
		static const float SIGHT_SEGMENT_LENGTH = 512.0f;
		float targetX = (SIGHT_SEGMENT_LENGTH * cos(angle)) + sourcePosition.getX();
		float targetY = (SIGHT_SEGMENT_LENGTH * sin(angle)) + sourcePosition.getY();
		Vector targetPosition = Vector(targetX, targetY);
		Graphics::get().drawSegment(sourcePosition, targetPosition, Color(0.0f, 1.0f, 1.0f, 0.3f));
	}

	void Sight::drawFieldOfView(const Point &sourcePosition, Orientation::Enum sourceOrientation) const
	{
		drawFieldOfViewSegment(_sightCenter + (_sightSize / 2.0f), sourceOrientation, sourcePosition);
		drawFieldOfViewSegment(_sightCenter - (_sightSize / 2.0f), sourceOrientation, sourcePosition);
	}

	void Sight::drawDebugInfo(void) const
	{
		const Point& sourcePosition = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Orientation::Enum sourceOrientation = *(Orientation::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		const Point& targetPosition = *(Point*)EntitiesManager::get().sendMessageToEntity(0, Message(MessageID::GET_POSITION));

		drawFieldOfView(sourcePosition, sourceOrientation);
		if(_pointOfIntersection != Point::Zero)
			Graphics::get().drawSegment(sourcePosition, _pointOfIntersection, _isSeeing ? Color::Green : Color::Red);
	}
}