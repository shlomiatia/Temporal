#include "Sight.h"
#include "Grid.h"
#include "Math.h"
#include "Graphics.h"
#include "Shapes.h"
#include "MessageUtils.h"
#include "PhysicsEnums.h"
#include "Fixture.h"

namespace Temporal
{
	const Hash Sight::TYPE = Hash("sight");

	static const float RAYS = 3.0f;
	static const int COLLISION_MASK1 = CollisionCategory::OBSTACLE | CollisionCategory::PLAYER;
	static const int COLLISION_MASK2 = CollisionCategory::OBSTACLE | CollisionCategory::DEAD;

	void Sight::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			checkLineOfSight(COLLISION_MASK1);
			checkLineOfSight(COLLISION_MASK2);
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			drawDebugInfo();
		}
	}

	void Sight::checkLineOfSight(int collisionMask)
	{
		Side::Enum sourceSide = getOrientation(*this);
		Vector position = getPosition(*this) + _sightOffset.multiplyComponents(sourceSide, 1.0f);
		
		Vector sourceDirection = Vector(sourceSide, 0.0f);
		int sourceCollisionGroup = getIntParam(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
		for (float i = -_sightSize.getY(); i <= _sightSize.getY(); i += (_sightSize.getY() * 2.0f) / (RAYS - 1.0f))
		{
			Vector sourcePosition = position + Vector(0.0f, i);
			RayCastResult result;
			if (getEntity().getManager().getGameState().getGrid().cast(sourcePosition, sourceDirection, result, collisionMask, sourceCollisionGroup))
			{
				if (result.getFixture().getCategory() == CollisionCategory::OBSTACLE)
					continue;

				void* isVisible = getEntity().getManager().sendMessageToEntity(result.getFixture().getEntityId(), Message(MessageID::IS_VISIBLE));
				if (isVisible && !getBoolParam(isVisible))
					continue;

				if (fabsf(result.getDirectedSegment().getVector().getY()) > _sightSize.getY() || fabsf(result.getDirectedSegment().getVector().getX()) > _sightSize.getX())
					continue;

				raiseMessage(Message(MessageID::LINE_OF_SIGHT, &result));
				return;
				
			}
		}
	}

	void drawFieldOfViewSegment(const Vector &sourcePosition, Side::Enum sourceSide, const Vector& delta)
	{
		OBB line(sourcePosition + Vector(delta.getX() / 2.0f * sourceSide, delta.getY()), Vector(sourceSide, 0.0f), Vector(delta.getX() / 2.0f, 0.5f));
		Graphics::get().getLinesSpriteBatch().add(line, Color(0.0f, 1.0f, 1.0f, 0.3f));
	}

	void Sight::drawFieldOfView(const Vector &sourcePosition, Side::Enum sourceSide) const
	{
		for (float i = -_sightSize.getY(); i <= _sightSize.getY(); i += (_sightSize.getY() * 2.0f) / (RAYS - 1.0f))
			drawFieldOfViewSegment(sourcePosition, sourceSide, Vector(_sightSize.getX(), i));
	}

	void Sight::drawDebugInfo() const
	{
		Side::Enum sourceSide = getOrientation(*this);
		Vector sourcePosition = getPosition(*this) + _sightOffset.multiplyComponents(sourceSide, 1.0f);
		
		drawFieldOfView(sourcePosition, sourceSide);
	}
}