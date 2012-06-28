#include "Sight.h"
#include "Grid.h"
#include "StaticBody.h"
#include "Segment.h"
#include "Math.h"
#include "Message.h"
#include "EntitiesManager.h"
#include "Graphics.h"
#include "DirectedSegment.h"

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
		Side::Enum sourceSide = *(Side::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		const Point& targetPosition = *(Point*)EntitiesManager::get().sendMessageToEntity(_targetID, Message(MessageID::GET_POSITION));

		// Check orientation
		if(differentSign(targetPosition.getX() - sourcePosition.getX(), (float)sourceSide))
			return;

		// Check field of view
		DirectedSegment directedSegment(sourcePosition.getX(), sourcePosition.getY(), targetPosition.getX(), targetPosition.getY());
		float angle = directedSegment.getVector().getAngle();
		float sightCenter = sourceSide == Side::RIGHT ? _sightCenter : mirroredAngle(_sightCenter);
		float distance = minAnglesDistance(sightCenter, angle);
		if(distance > _sightSize / 2.0f) return;
		
		int* myPeriodPointer = (int*)sendMessageToOwner(Message(MessageID::GET_PERIOD));
		int myCollisionFilter = myPeriodPointer == NULL ? 0 : *myPeriodPointer ;
		int* targetPeriodPointer = (int*)EntitiesManager::get().sendMessageToEntity(_targetID, Message(MessageID::GET_PERIOD));
		int targetCollisionFilter = targetPeriodPointer == NULL ? 0 : *targetPeriodPointer;
		if(myCollisionFilter != 0 && targetCollisionFilter != 0 && (myCollisionFilter & targetCollisionFilter) == 0)
			return;
		_isSeeing = Grid::get().cast(directedSegment, myCollisionFilter, _pointOfIntersection);
		
		if(_isSeeing)
			sendMessageToOwner(Message(MessageID::LINE_OF_SIGHT));
	}



	void drawFieldOfViewSegment(float angle, Side::Enum sourceSide, const Point &sourcePosition)
	{
		if(sourceSide == Side::LEFT)
			angle = PI - angle;
		static const float SIGHT_SEGMENT_LENGTH = 512.0f;
		float targetX = (SIGHT_SEGMENT_LENGTH * cos(angle)) + sourcePosition.getX();
		float targetY = (SIGHT_SEGMENT_LENGTH * sin(angle)) + sourcePosition.getY();
		Point targetPosition = Point(targetX, targetY);
		Graphics::get().draw(Segment(sourcePosition, targetPosition), Color(0.0f, 1.0f, 1.0f, 0.3f));
	}

	void Sight::drawFieldOfView(const Point &sourcePosition, Side::Enum sourceSide) const
	{
		drawFieldOfViewSegment(_sightCenter + (_sightSize / 2.0f), sourceSide, sourcePosition);
		drawFieldOfViewSegment(_sightCenter - (_sightSize / 2.0f), sourceSide, sourcePosition);
	}

	void Sight::drawDebugInfo(void) const
	{
		const Point& sourcePosition = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Side::Enum sourceSide = *(Side::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));

		drawFieldOfView(sourcePosition, sourceSide);
		if(_pointOfIntersection != Point::Zero)
			Graphics::get().draw(Segment(sourcePosition, _pointOfIntersection), _isSeeing ? Color::Green : Color::Red);
	}
}