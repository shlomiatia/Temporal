#include "Renderer.h"
#include "Graphics.h"
#include "SpriteSheet.h"
#include "Hash.h"
#include "Serialization.h"
#include "SceneNode.h"
#include "MessageParams.h"

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	void Renderer::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::SET_COLOR)
		{
			_color = *static_cast<Color*>(message.getParam());
		}
		else if(message.getID() == MessageID::DRAW)
		{
			VisualLayer::Enum layer = *static_cast<VisualLayer::Enum*>(message.getParam());
			if(_layer == layer)
				draw();			
		}
	}
	
	void Renderer::draw() const
	{
		Point position = Vector::Zero;
		Message getDrawPosition(MessageID::GET_DRAW_POSITION, &position);
		sendMessageToOwner(getDrawPosition);
		if(position == Vector::Zero)
			position = *static_cast<Point*>(sendMessageToOwner(Message(MessageID::GET_POSITION)));

		Side::Enum spritesheetOrientation = _spritesheet.getOrientation();
		const Side::Enum* entityOrientation = (const Side::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		Side::Enum orientation;
		if(entityOrientation == NULL || *entityOrientation == Side::NONE)
			orientation = spritesheetOrientation;
		else
			orientation = *entityOrientation;
		
		/*int* myPeriodPointer = static_cast<int*>(sendMessageToOwner(Message(MessageID::GET_PERIOD));
		int myCollisionFilter = myPeriodPointer == NULL ? 0 : *myPeriodPointer ;
		int* targetPeriodPointer = static_cast<int*>(EntitiesManager::get().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_PERIOD));
		int targetCollisionFilter = targetPeriodPointer == NULL ? 0 : *targetPeriodPointer;
		Color color = myCollisionFilter == targetCollisionFilter ? _color : Color(_color.getR(), _color.getG(), _color.getB(), 0.2f);*/

		_root->setMirrored(orientation != spritesheetOrientation);
		_root->setTranslation(position);
		Graphics::get().draw(*_root, _spritesheet, _color);
	}

	Component* Renderer::clone() const
	{
		return new Renderer(_spritesheet, _layer, _root->clone(), _color);
	}

}