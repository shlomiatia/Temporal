#include "Renderer.h"
#include "Graphics.h"
#include "SpriteSheet.h"
#include "Hash.h"
#include "Serialization.h"
#include "Message.h"
#include "EntitiesManager.h"

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	static const Hash SPRITE_GROUP_SERIALIZATION = Hash("REN_SER_SPR_GRP");
	static const Hash SPRITE_SERIALIZATION = Hash("REN_SER_SPR");

	void Renderer::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_SPRITE_GROUP)
		{
			message.setParam((void*)&_spritesheet.get(_spriteGroupID));
		}
		else if(message.getID() == MessageID::GET_SPRITE)
		{
			message.setParam((void*)&_spritesheet.get(_spriteGroupID).get(_spriteID));
		}
		else if(message.getID() == MessageID::SET_SPRITE_GROUP_ID)
		{
			_spriteGroupID = *(Hash*)message.getParam();
		}
		else if(message.getID() == MessageID::SET_SPRITE_ID)
		{
			_spriteID = *(int*)message.getParam();
		}
		else if(message.getID() == MessageID::SET_COLOR)
		{
			_color = *(Color*)message.getParam();
		}
		else if(message.getID() == MessageID::DRAW)
		{
			VisualLayer::Enum layer = *(VisualLayer::Enum*)message.getParam();
			if(_layer == layer)
				draw();			
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *(Serialization*)message.getParam();
			serialization.serialize(SPRITE_GROUP_SERIALIZATION, _spriteGroupID);
			serialization.serialize(SPRITE_SERIALIZATION, _spriteID);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *(const Serialization*)message.getParam();
			_spriteGroupID = Hash(serialization.deserializeUInt(SPRITE_GROUP_SERIALIZATION));
			_spriteID = (Side::Enum)serialization.deserializeInt(SPRITE_SERIALIZATION);
		}
	}
	
	void Renderer::draw(void) const
	{
		Point position = Vector::Zero;
		Message getDrawPosition(MessageID::GET_DRAW_POSITION, &position);
		sendMessageToOwner(getDrawPosition);
		if(position == Vector::Zero)
			position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));

		Side::Enum spritesheetOrientation = _spritesheet.getOrientation();
		const Side::Enum* entityOrientation = (const Side::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		Side::Enum orientation;
		if(entityOrientation == NULL || *entityOrientation == Side::NONE)
			orientation = spritesheetOrientation;
		else
			orientation = *entityOrientation;
		
		bool mirrored = orientation != spritesheetOrientation;
		const Sprite& sprite = _spritesheet.get(_spriteGroupID).get(_spriteID);
		const Vector& offset = sprite.getOffset();
		float screenLocationX = position.getX() - orientation * spritesheetOrientation * offset.getVx();
		float screenLocationY = position.getY() - offset.getVy();

		Point screenLocation(screenLocationX, screenLocationY);

		int* myPeriodPointer = (int*)sendMessageToOwner(Message(MessageID::GET_PERIOD));
		int myCollisionFilter = myPeriodPointer == NULL ? 0 : *myPeriodPointer ;
		int* targetPeriodPointer = (int*)EntitiesManager::get().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_PERIOD));
		int targetCollisionFilter = targetPeriodPointer == NULL ? 0 : *targetPeriodPointer;
		Color color = myCollisionFilter == targetCollisionFilter ? _color : Color(_color.getR(), _color.getG(), _color.getB(), 0.2f);

		Graphics::get().draw(_spritesheet.getTexture(), sprite.getBounds(), screenLocation, mirrored, color);
	}

	const Hash& Renderer::getFirstSpriteGroupID(const SpriteSheet& spritesheet)
	{
		return spritesheet.getFirstSpriteGroupID();
	}

	Component* Renderer::clone(void) const
	{
		return new Renderer(_spritesheet, _layer, _color);
	}
}