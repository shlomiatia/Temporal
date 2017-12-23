#include "Door.h"
#include "PhysicsEnums.h"
#include "SceneNode.h"
#include "MessageUtils.h"
#include "Shapes.h"
#include "Renderer.h"
#include "SpriteSheet.h"
#include "Texture.h"
#include "NavigationGraph.h"
#include "NavigationNode.h"

namespace Temporal
{
	void Door::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::GAME_STATE_READY)
		{
			set();
		}
		else if (message.getID() == MessageID::TOGGLE_ACTIVATION)
		{
			_opened = !_opened;
			set();
		}
		else if (message.getID() == MessageID::IS_ACTIVATED)
		{
			message.setParam(&_opened);
		}
	}

	void Door::set()
	{
		int collisionCategory = !_opened ? CollisionCategory::OBSTACLE : -1;
		raiseMessage(Message(MessageID::SET_COLLISION_CATEGORY, &collisionCategory));
		Renderer& renderer = *static_cast<Renderer*>(getEntity().get(ComponentsIds::RENDERER));
		const OBB& shape = getShape(*this);
		if (!_opened)
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

		int group = getIntParam(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
		NavigationGraph& navigationGraph = getEntity().getManager().getGameState().getNavigationGraph();
		NavigationNode* navigationNode = navigationGraph.getNode(getEntity().getId(), group);
		if (navigationNode)
		{
			navigationNode->setDisabled(!_opened);
		}
	}
}