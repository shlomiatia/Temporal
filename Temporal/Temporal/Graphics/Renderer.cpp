#include "Renderer.h"
#include "Graphics.h"
#include <assert.h>

namespace Temporal
{
	void Renderer::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::GET_SPRITE_GROUP)
		{
			message.setParam(&_spritesheet.get(_spriteGroupID));
		}
		else if(message.getID() == MessageID::GET_SPRITE)
		{
			message.setParam(&_spritesheet.get(_spriteGroupID).get(_spriteID));
		}
		else if(message.getID() == MessageID::SET_SPRITE_GROUP_ID)
		{
			_spriteGroupID = *(const int* const)message.getParam();
		}
		else if(message.getID() == MessageID::SET_SPRITE_ID)
		{
			_spriteID = *(const int* const)message.getParam();
		}
		else if(message.getID() == MessageID::SET_COLOR)
		{
			_color = *(const Color* const)message.getParam();
		}
		else if(message.getID() == MessageID::DRAW)
		{
			VisualLayer::Enum layer = *(const VisualLayer::Enum* const)message.getParam();
			if(_layer == layer)
				draw();			
		}
	}
	
	void Renderer::draw(void) const
	{
		Vector position = Vector::Zero;
		Message getDrawPosition(MessageID::GET_DRAW_POSITION, &position);
		sendMessageToOwner(getDrawPosition);
		if(position == Vector::Zero)
			position = *(const Vector* const)sendQueryMessageToOwner(Message(MessageID::GET_POSITION));

		Orientation::Enum spritesheetOrientation = _spritesheet.getOrientation();
		const Orientation::Enum* const entityOrientation = (const Orientation::Enum* const)sendQueryMessageToOwner(Message(MessageID::GET_ORIENTATION));
		Orientation::Enum orientation;
		if(entityOrientation == NULL || *entityOrientation == Orientation::NONE)
			orientation = spritesheetOrientation;
		else
			orientation = *entityOrientation;
		
		bool mirrored = orientation != spritesheetOrientation;
		const Sprite& sprite = _spritesheet.get(_spriteGroupID).get(_spriteID);
		const Vector& offset = sprite.getOffset();
		float screenLocationX = position.getX() - orientation * spritesheetOrientation * offset.getX();
		float screenLocationY = position.getY() - offset.getY();

		Vector screenLocation(screenLocationX, screenLocationY);

		Graphics::get().drawTexture(_spritesheet.getTexture(), sprite.getBounds(), screenLocation, mirrored, _color);
	}
}