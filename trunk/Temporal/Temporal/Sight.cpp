#include "Sight.h"
#include "Grid.h"
#include "Segment.h"
#include "Math.h"
#include "Graphics.h"
#include "Shapes.h"
#include "MessageUtils.h"
#include "PhysicsEnums.h"
#include "CollisionFilter.h"
#include "Fixture.h"

namespace Temporal
{
	const Hash Sight::TYPE = Hash("sight");

	static const int COLLISION_MASK = CollisionCategory::OBSTACLE | CollisionCategory::COVER | CollisionCategory::PLAYER;

	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	void Sight::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			_filter = static_cast<const CollisionFilter*>(getEntity().get(CollisionFilter::TYPE));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			checkLineOfSight();
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			drawDebugInfo();
		}
	}

	void Sight::checkLineOfSight()
	{
		_pointOfIntersection = Vector::Zero;
		_isSeeing = false;

		int sourceCollisionGroup = *static_cast<int*>(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
		int targetCollisionGroup = *static_cast<int*>(getEntity().getManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_COLLISION_GROUP)));
		if(sourceCollisionGroup != -1 &&
		   targetCollisionGroup != -1 &&
		   sourceCollisionGroup != targetCollisionGroup)
		   return;

		const Vector& sourcePosition = getPosition(*this);
		Side::Enum sourceSide = getOrientation(*this);
		const Vector& targetPosition = *static_cast<Vector*>(getEntity().getManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_POSITION)));

		// Check orientation
		if(differentSign(targetPosition.getX() - sourcePosition.getX(), static_cast<float>(sourceSide)))
			return;

		void* isLit = getEntity().getManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::IS_LIT));
		if(isLit && !getBoolParam(isLit))
			return;

		// Check field of view
		Vector vector = targetPosition - sourcePosition;
		float angle = vector.getAngle();
		float sightCenter = sourceSide == Side::RIGHT ? _sightCenter : mirroredAngle(_sightCenter);
		float distance = minAnglesDistance(sightCenter, angle);
		if(distance > _sightSize / 2.0f) return;

		RayCastResult result;
		if(getEntity().getManager().getGameState().getGrid().cast(sourcePosition, vector.normalize(), result, COLLISION_MASK, _filter->getGroup()))
		{
			_pointOfIntersection = result.getPoint();
			if(result.getFixture().getEntityId() == PLAYER_ENTITY)
				_isSeeing = true;
		}
		
		if(_isSeeing)
			raiseMessage(Message(MessageID::LINE_OF_SIGHT));
	}

	void drawFieldOfViewSegment(float angle, Side::Enum sourceSide, const Vector &sourcePosition)
	{
		if(sourceSide == Side::LEFT)
			angle = PI - angle;
		static const float SIGHT_SEGMENT_LENGTH = 512.0f;
		float targetX = (SIGHT_SEGMENT_LENGTH * cos(angle)) + sourcePosition.getX();
		float targetY = (SIGHT_SEGMENT_LENGTH * sin(angle)) + sourcePosition.getY();
		Vector targetPosition = Vector(targetX, targetY);
		//Graphics::get().draw(SegmentPP(sourcePosition, targetPosition), Color(0.0f, 1.0f, 1.0f, 0.3f));
	}

	void Sight::drawFieldOfView(const Vector &sourcePosition, Side::Enum sourceSide) const
	{
		drawFieldOfViewSegment(_sightCenter + (_sightSize / 2.0f), sourceSide, sourcePosition);
		drawFieldOfViewSegment(_sightCenter - (_sightSize / 2.0f), sourceSide, sourcePosition);
	}

	void Sight::drawDebugInfo() const
	{
		const Vector& sourcePosition = getPosition(*this);
		Side::Enum sourceSide = getOrientation(*this);

		drawFieldOfView(sourcePosition, sourceSide);
		if(_pointOfIntersection != Vector::Zero)
		{
			//Graphics::get().draw(SegmentPP(sourcePosition, _pointOfIntersection), _isSeeing ? Color::Green : Color::Red);
		}
	}
}