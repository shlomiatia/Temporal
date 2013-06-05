#include "Laser.h"
#include "Shapes.h"
#include "Grid.h"
#include "Math.h"
#include "Vector.h"
#include "Serialization.h"
#include "MessageUtils.h"
#include "Fixture.h"
#include "PhysicsEnums.h"
#include "Renderer.h"
#include "SceneNode.h"

namespace Temporal
{
	static const int COLLISION_MASK = CollisionCategory::OBSTACLE | CollisionCategory::PLAYER;

	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	void Laser::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			Renderer* renderer = static_cast<Renderer*>(getEntity().get(ComponentType::RENDERER));
			_root = &renderer->getRoot();
		}
		else if(message.getID() == MessageID::LEVEL_INIT)
		{
			const Segment& segment = *static_cast<Segment*>(EntitiesManager::get().sendMessageToEntity(_platformID, Message(MessageID::GET_SHAPE)));
			Vector position = segment.getNaturalOrigin();
			position.setY(position.getY() - 1.0f);
			raiseMessage(Message(MessageID::SET_POSITION, &position));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			update(framePeriod);
		}
	}

	void Laser::update(float framePeriod)
	{
		const Vector& position = getPosition(*this);
		const Segment& segment = *static_cast<Segment*>(EntitiesManager::get().sendMessageToEntity(_platformID, Message(MessageID::GET_SHAPE)));
		Vector platformDirection = segment.getNaturalVector().normalize();
		float movementAmount = LASER_SPEED_PER_SECOND * framePeriod;
		Vector maxPoint = Vector::Zero;
		if(_isPositiveDirection)
		{
			maxPoint = segment.getNaturalTarget();
		}
		else
		{
			maxPoint = segment.getNaturalOrigin();
			platformDirection = -platformDirection;
		}
		Vector movement = platformDirection * movementAmount;
		Vector currDiff = maxPoint - position;
		Vector nextDiff = currDiff + movement;
		if(!sameSign(currDiff.getX(), nextDiff.getX()) || !sameSign(currDiff.getY(), nextDiff.getY()))
		{
			_isPositiveDirection = !_isPositiveDirection;
			movement *= -1.0f;
		}
		Vector newPosition = position + movement;
		raiseMessage(Message(MessageID::SET_POSITION, &newPosition));
		RayCastResult result;
		int group = *static_cast<int*>(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
		if(Grid::get().cast(newPosition, Vector(0.0f, -1.0f), result, COLLISION_MASK, group))
		{
			Color color = result.getFixture().getEntityId() == PLAYER_ENTITY ? Color::Green : Color::Red;
			raiseMessage(Message(MessageID::SET_COLOR, &color));
			float vector = (result.getPoint().getY() - position.getY());
			_root->setTranslation(Vector(0.0f, vector / 2.0f));
			_root->setScale(Vector(1.0f, abs(vector)));

		}
	}
}