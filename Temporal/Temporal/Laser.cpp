#include "Laser.h"
#include "Shapes.h"
#include "Grid.h"
#include "Math.h"
#include "Vector.h"
#include "MessageUtils.h"
#include "Fixture.h"
#include "PhysicsEnums.h"
#include "SceneNode.h"

namespace Temporal
{
	const Hash LASER_SCENE_NODE = Hash("SCN_LASER");

	static const int COLLISION_MASK = CollisionCategory::OBSTACLE | CollisionCategory::PLAYER | CollisionCategory::CHARACTER;

	void Laser::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_POST_INIT)
		{
			_direction = -_platformSearchDirection.getRightNormal();
			Vector impulse = _speedPerSecond * _direction;
			raiseMessage(Message(MessageID::SET_IMPULSE, &impulse));
			setColor();
		}
		else if (message.getID() == MessageID::COLLISIONS_CORRECTED)
		{
			_direction *= -1.0f;
		}
		else if (message.getID() == MessageID::GAME_STATE_READY)
		{
			RayCastResult result;
			const Vector& position = getPosition(*this);
			int group = getIntParam(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
			if (getEntity().getManager().getGameState().getGrid().cast(position, _platformSearchDirection, result, CollisionCategory::OBSTACLE, group))
			{
				Hash id = result.getFixture().getEntityId();
				Entity* entity = getEntity().getManager().getEntity(id);
				_platform = static_cast<const OBB*>(entity->handleMessage(Message(MessageID::GET_SHAPE)));
				Vector position = result.getDirectedSegment().getTarget() - _platformSearchDirection;
				raiseMessage(Message(MessageID::SET_POSITION, &position));
			}
			else
			{
				abort();
			}
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			update(framePeriod);
		}
		else if (message.getID() == MessageID::TOGGLE_ACTIVATION)
		{
			_friendly = !_friendly;
			setColor();
				
		}
		else if (message.getID() == MessageID::IS_ACTIVATED)
		{
			message.setParam(&_friendly);
		}
	}

	void Laser::setColor()
	{
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

	void Laser::setLength(float length)
	{
		SceneNode* root = static_cast<SceneNode*>(raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
		root = root->get(LASER_SCENE_NODE);
		Vector translation = -_platformSearchDirection * length / 2.0f;
		root->setTranslation(translation);
		root->setRotation(AngleUtils::radiansToDegrees(_direction.getAngle()));
		root->setScale(Vector(1.0f, length));
		
	}

	void Laser::update(float framePeriod)
	{
		const OBB& shape = getShape(*this);
		RayCastResult result;
		const Vector& position = getPosition(*this);
		int group = getIntParam(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
		if (getEntity().getManager().getGameState().getGrid().cast(position, -_platformSearchDirection, result, COLLISION_MASK, group))
		{
			float length = result.getDirectedSegment().getVector().getLength();
			
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

		Vector nextPosition = position + _speedPerSecond * _direction * framePeriod;
		Vector normal = _platformSearchDirection.getRightNormal();
		float dp = nextPosition * normal;
		Vector position1 = _platform->getCenter() - _platform->getRadius();
		float dp1 = position1 * normal;
		Vector position2 = _platform->getCenter() + _platform->getRadius();
		float dp2 = position2 * normal;
		
		if (dp1 > dp2)
		{
			std::swap(dp1, dp2);
		}
		
		if (dp < dp1 || dp > dp2)
		{
			_direction *= -1.0f;
			
		}
		Vector impulse = _speedPerSecond * _direction;
		raiseMessage(Message(MessageID::SET_IMPULSE, &impulse));

	}
}