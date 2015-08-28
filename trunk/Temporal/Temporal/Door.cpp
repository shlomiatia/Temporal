#include "Door.h"
#include "PhysicsEnums.h"
#include "SceneNode.h"
#include "MessageUtils.h"
#include "Shapes.h"
#include "Renderer.h"
#include "SpriteSheet.h"
#include "Texture.h"

namespace Temporal
{
	const Hash Door::TYPE = Hash("door");

	void Door::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_POST_INIT)
		{
			set();
		}
		else if (message.getID() == MessageID::ACTIVATE)
		{
			_closed = !_closed;
			set();
		}
	}

	void Door::set()
	{
		int collisionCategory = _closed ? CollisionCategory::OBSTACLE : -1;
		raiseMessage(Message(MessageID::SET_COLLISION_CATEGORY, &collisionCategory));
		Renderer& renderer = *static_cast<Renderer*>(getEntity().get(Renderer::TYPE));
		const OBB& shape = getShape(*this);
		if (_closed)
		{
			renderer.getRootSceneNode().setTranslation(Vector::Zero);
			renderer.getRootSceneNode().setScale(Vector(shape.getRadius().getX() / (renderer.getSpriteSheet().getTexture().getSize().getX() / 2.0f), 
														shape.getRadius().getY() / (renderer.getSpriteSheet().getTexture().getSize().getY() / 2.0f)));
		}
		else
		{
			
			renderer.getRootSceneNode().setTranslation(Vector(0.0f, shape.getRadiusY() - shape.getRadiusY() / 10.0f));
			renderer.getRootSceneNode().setScale(Vector(1.0f, 0.1f));
		}
		
	}
}