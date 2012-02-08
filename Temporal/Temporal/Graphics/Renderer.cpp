#include "Renderer.h"
#include "Graphics.h"
#include <assert.h>

namespace Temporal
{
	Renderer::Renderer(const SpriteSheet& spritesheet, VisualLayer::Enum layer, int spriteGroupID, int spriteID)
		: _spritesheet(spritesheet), _layer(layer), _spriteGroupID(spriteGroupID), _spriteID(spriteID)
	{
	}

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
			_spriteGroupID = message.getParam<int>();
		}
		else if(message.getID() == MessageID::SET_SPRITE_ID)
		{
			_spriteID = message.getParam<int>();
		}
		else if(message.getID() == MessageID::DRAW)
		{
			VisualLayer::Enum layer = message.getParam<VisualLayer::Enum>();
			if(_layer == layer)
				draw();			
		}
	}
	
	void Renderer::draw(void) const
	{
		Orientation::Enum spritesheetOrientation = _spritesheet.getOrientation();
		Vector position(Vector::Zero);
		Message getDrawPosition(MessageID::GET_DRAW_POSITION, &position);
		sendMessageToOwner(getDrawPosition);
		if(position == Vector::Zero)
			position = sendQueryMessageToOwner<Vector>(Message(MessageID::GET_POSITION));
		Message getOrientation(MessageID::GET_ORIENTATION);
		sendMessageToOwner(getOrientation);

		// TODO: Find a solution for output message parameters
		Orientation::Enum orientation = getOrientation.isNullParam() ? spritesheetOrientation : getOrientation.getParam<Orientation::Enum>();
		if(orientation == Orientation::NONE)
			orientation = spritesheetOrientation;
		
		bool mirrored = orientation != spritesheetOrientation;
		const Sprite& sprite = _spritesheet.get(_spriteGroupID).get(_spriteID);
		const Vector& offset = sprite.getOffset();
		float screenLocationX = position.getX() - orientation * spritesheetOrientation * offset.getX();
		float screenLocationY = position.getY() - offset.getY();

		Vector screenLocation(screenLocationX, screenLocationY);

		Graphics::get().drawTexture(_spritesheet.getTexture(), sprite.getBounds(), screenLocation, mirrored);
	}
}