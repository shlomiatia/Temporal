#include "Laser.h"
#include "Shapes.h"
#include "Grid.h"
#include "Math.h"
#include "Vector.h"
#include "Serialization.h"
#include "MessageUtils.h"
#include "Fixture.h"
#include "PhysicsEnums.h"
#include "SceneNode.h"

namespace Temporal
{
	const Hash Laser::TYPE = Hash("laser");

	static const float SPEED_PER_SECOND = 125.0f;
	static const int COLLISION_MASK = CollisionCategory::OBSTACLE | CollisionCategory::PLAYER;
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	void Laser::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			raiseMessage(Message(MessageID::SET_IMPULSE, &Vector(SPEED_PER_SECOND, 0.0f)));
		}
		else if (message.getID() == MessageID::BODY_COLLISION)
		{
			const Vector& collision = getVectorParam(message.getParam());
			Side::Enum side = collision.getX() < 0.0f ? Side::RIGHT : Side::LEFT;
			raiseMessage(Message(MessageID::SET_IMPULSE, &Vector(SPEED_PER_SECOND * side, 0.0f)));
		}
		else if (message.getID() == MessageID::LEVEL_INIT)
		{
			RayCastResult result;
			const Vector& position = getPosition(*this);
			int group = getIntParam(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
			if (getEntity().getManager().getGameState().getGrid().cast(position, Vector(0.0f, 1.0f), result, CollisionCategory::OBSTACLE, group))
			{
				Hash id = result.getFixture().getEntityId();
				Entity* entity = getEntity().getManager().getEntity(id);
				_platform = static_cast<const OBB*>(entity->handleMessage(Message(MessageID::GET_SHAPE)));
				raiseMessage(Message(MessageID::SET_POSITION, &Vector(position.getX(), _platform->getBottom() - 1.0f)));
			}
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			update(framePeriod);
		}
	}

	void Laser::update(float framePeriod)
	{
		const OBB& shape = getShape(*this);
		RayCastResult result;
		const Vector& position = getPosition(*this);
		int group = getIntParam(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
		if (getEntity().getManager().getGameState().getGrid().cast(position, Vector(0.0f, -1.0f), result, COLLISION_MASK, group))
		{
			float vector = (result.getPoint().getY() - position.getY());
			SceneNode* root = static_cast<SceneNode*>(raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			root->setTranslation(Vector(0.0f, vector / 2.0f));
			root->setScale(Vector(1.0f, abs(vector)));
			if (result.getFixture().getEntityId() == PLAYER_ENTITY)
			{
				Entity* takedownEntity = getEntity().getManager().getEntity(PLAYER_ENTITY);
				takedownEntity->handleMessage(Message(MessageID::DIE));
			}
		}
		if (position.getX() - shape.getRadiusX() - SPEED_PER_SECOND * framePeriod < _platform->getLeft())
		{
			raiseMessage(Message(MessageID::SET_IMPULSE, &Vector(SPEED_PER_SECOND, 0.0f)));
		}
		else if (position.getX() + shape.getRadiusX() + SPEED_PER_SECOND * framePeriod > _platform->getRight())
		{
			raiseMessage(Message(MessageID::SET_IMPULSE, &Vector(-SPEED_PER_SECOND, 0.0f)));
		}
	}
}