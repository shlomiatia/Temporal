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

	static const float SPEED_PER_SECOND = 128.0f;
	static const int COLLISION_MASK = CollisionCategory::OBSTACLE | CollisionCategory::PLAYER | CollisionCategory::CHARACTER;

	void Laser::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			raiseMessage(Message(MessageID::SET_IMPULSE, &Vector(SPEED_PER_SECOND, 0.0f)));
		}
		else if (message.getID() == MessageID::BODY_COLLISION)
		{
			raiseMessage(Message(MessageID::FLIP_ORIENTATION));
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
		else if (message.getID() == MessageID::ACTIVATE)
		{
			_friendly = !_friendly;
			Color color = Color::White;
			if (_friendly)
			{
				color = Color::Green;
			}
			else
			{
				color = Color::Red;
			}
			raiseMessage(Message(MessageID::SET_COLOR, &color));
				
		}
	}

	void Laser::setLength(float length)
	{
		SceneNode* root = static_cast<SceneNode*>(raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
		root->setTranslation(Vector(0.0f, length / 2.0f));
		root->setScale(Vector(1.0f, abs(length)));
	}

	void Laser::update(float framePeriod)
	{
		const OBB& shape = getShape(*this);
		RayCastResult result;
		const Vector& position = getPosition(*this);
		int group = getIntParam(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
		if (getEntity().getManager().getGameState().getGrid().cast(position, Vector(0.0f, -1.0f), result, COLLISION_MASK, group))
		{
			float length = result.getPoint().getY() - position.getY();
			
			
			if (result.getFixture().getCategory() == CollisionCategory::PLAYER && !_friendly ||
				result.getFixture().getCategory() == CollisionCategory::CHARACTER && _friendly)
			{
				getEntity().getManager().sendMessageToEntity(result.getFixture().getEntityId(), Message(MessageID::DIE));
			}
			else if (result.getFixture().getCategory() == CollisionCategory::PLAYER && _friendly ||
					 result.getFixture().getCategory() == CollisionCategory::CHARACTER && !_friendly)
			{
   				length = 0.0f;
			}

			setLength(length);
		}
		Side::Enum orientation = getOrientation(*this);
		if (orientation == Side::LEFT && position.getX() - shape.getRadiusX() - SPEED_PER_SECOND * framePeriod <= _platform->getLeft())
		{
			raiseMessage(Message(MessageID::FLIP_ORIENTATION));
			
		}
		else if (orientation == Side::RIGHT && position.getX() + shape.getRadiusX() + SPEED_PER_SECOND * framePeriod >= _platform->getRight())
		{
			raiseMessage(Message(MessageID::FLIP_ORIENTATION));
		}
		raiseMessage(Message(MessageID::SET_IMPULSE, &Vector(SPEED_PER_SECOND, 0.0f)));

	}
}