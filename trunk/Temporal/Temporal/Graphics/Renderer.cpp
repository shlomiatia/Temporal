#include "Renderer.h"
#include "Graphics.h"
#include "SpriteSheet.h"
#include <Temporal\Game\Message.h>

namespace Temporal
{
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
			_spriteGroupID = *(int*)message.getParam();
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
	}
	
	void Renderer::draw(void) const
	{
		Point position = Vector::Zero;
		Message getDrawPosition(MessageID::GET_DRAW_POSITION, &position);
		sendMessageToOwner(getDrawPosition);
		if(position == Vector::Zero)
			position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));

		Orientation::Enum spritesheetOrientation = _spritesheet.getOrientation();
		const Orientation::Enum* entityOrientation = (const Orientation::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		Orientation::Enum orientation;
		if(entityOrientation == NULL || *entityOrientation == Orientation::NONE)
			orientation = spritesheetOrientation;
		else
			orientation = *entityOrientation;
		
		bool mirrored = orientation != spritesheetOrientation;
		const Sprite& sprite = _spritesheet.get(_spriteGroupID).get(_spriteID);
		const Vector& offset = sprite.getOffset();
		float screenLocationX = position.getX() - orientation * spritesheetOrientation * offset.getVx();
		float screenLocationY = position.getY() - offset.getVy();

		Point screenLocation(screenLocationX, screenLocationY);

		Graphics::get().draw(_spritesheet.getTexture(), sprite.getBounds(), screenLocation, mirrored, _color);
	}
}