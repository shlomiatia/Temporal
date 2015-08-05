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

	static const int COLLISION_MASK = CollisionCategory::OBSTACLE | CollisionCategory::PLAYER;

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

		void* isVisible = getEntity().getManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::IS_VISIBLE));
		if (isVisible && !getBoolParam(isVisible))
			return;

		// Check field of view
		Vector vector = targetPosition - sourcePosition;
		if (fabsf(vector.getY()) > _sightSize) return;

		RayCastResult result;
		if(getEntity().getManager().getGameState().getGrid().cast(sourcePosition, vector.normalize(), result, COLLISION_MASK, _filter->getGroup()))
		{
			_pointOfIntersection = result.getPoint();
			if(result.getFixture().getEntityId() == PLAYER_ENTITY)
				_isSeeing = true;
		}
		
		if (_isSeeing)
		{
			float angle = vector.getAngle();
			if (sourceSide == Side::LEFT)
				angle = AngleUtils::radian().mirror(angle);
			raiseMessage(Message(MessageID::LINE_OF_SIGHT, &angle));
		}
	}

	void drawFieldOfViewSegment(const Vector &sourcePosition, Side::Enum sourceSide, float yDelta)
	{
		static const float SIGHT_SEGMENT_LENGTH = 512.0f;
		OBB line(sourcePosition + Vector(SIGHT_SEGMENT_LENGTH / 2.0f * sourceSide, yDelta), Vector(sourceSide, 0.0f), Vector(SIGHT_SEGMENT_LENGTH / 2.0f, 0.5f));
		Graphics::get().getLinesSpriteBatch().add(line, Color(0.0f, 1.0f, 1.0f, 0.3f));
	}

	void Sight::drawFieldOfView(const Vector &sourcePosition, Side::Enum sourceSide) const
	{
		drawFieldOfViewSegment(sourcePosition, sourceSide, -_sightSize);
		drawFieldOfViewSegment(sourcePosition, sourceSide, _sightSize);
	}

	void Sight::drawDebugInfo() const
	{
		Graphics::get().getLinesSpriteBatch().begin();
		const Vector& sourcePosition = getPosition(*this);
		Side::Enum sourceSide = getOrientation(*this);

		drawFieldOfView(sourcePosition, sourceSide);
		Vector radius = _pointOfIntersection - sourcePosition;

		if(_pointOfIntersection != Vector::Zero && radius != Vector::Zero)
		{
			Graphics::get().getLinesSpriteBatch().add(OBB(sourcePosition + radius / 2.0f, radius.normalize(), Vector(radius.getLength() / 2.0f, 0.5f)), _isSeeing ? Color::Green : Color::Red);
		}
		Graphics::get().getLinesSpriteBatch().end();
	}
}