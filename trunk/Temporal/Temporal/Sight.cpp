#include "Sight.h"
#include "Grid.h"
#include "StaticBody.h"
#include "Segment.h"
#include "Math.h"
#include "Graphics.h"
#include "Shapes.h"
#include "MessageUtils.h"
#include "PhysicsEnums.h"

namespace Temporal
{
	static const int COLLISION_MASK1 = Filter1::OBSTACLE | Filter1::COVER;

	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

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

	void Sight::checkLineOfSight()
	{
		_pointOfIntersection = Point::Zero;
		_isSeeing = false;

		void* isLit = EntitiesManager::get().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::IS_LIT));
		if(isLit != NULL && !*static_cast<bool*>(isLit))
			return;

		const Point& sourcePosition = *static_cast<Point*>(sendMessageToOwner(Message(MessageID::GET_POSITION)));
		Side::Enum sourceSide = *(Side::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		const Point& targetPosition = *static_cast<Point*>(EntitiesManager::get().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_POSITION)));

		// Check orientation
		if(differentSign(targetPosition.getX() - sourcePosition.getX(), static_cast<float>(sourceSide)))
			return;

		// Check field of view
		DirectedSegment directedSegment(sourcePosition.getX(), sourcePosition.getY(), targetPosition.getX(), targetPosition.getY());
		float angle = directedSegment.getVector().getAngle();
		float sightCenter = sourceSide == Side::RIGHT ? _sightCenter : mirroredAngle(_sightCenter);
		float distance = minAnglesDistance(sightCenter, angle);
		if(distance > _sightSize / 2.0f) return;
		
		_isSeeing = Grid::get().cast(directedSegment, _pointOfIntersection);
		
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

	void Sight::drawDebugInfo() const
	{
		const Point& sourcePosition = *static_cast<Point*>(sendMessageToOwner(Message(MessageID::GET_POSITION)));
		Side::Enum sourceSide = *(Side::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));

		drawFieldOfView(sourcePosition, sourceSide);
		if(_pointOfIntersection != Point::Zero)
			Graphics::get().draw(Segment(sourcePosition, _pointOfIntersection), _isSeeing ? Color::Green : Color::Red);
	}
}