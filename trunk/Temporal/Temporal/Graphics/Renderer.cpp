#include "Renderer.h"
#include "Graphics.h"
#include <assert.h>

namespace Temporal
{
	Renderer::Renderer(const SpriteSheet& spritesheet, int spriteGroupID, int spriteID)
		: _spritesheet(spritesheet), _spriteGroupID(spriteGroupID), _spriteID(spriteID)
	{
	}

	void Renderer::handleMessage(Message& message)
	{
		switch(message.getID())
		{
			case(MessageID::GET_SPRITE_GROUP):
			{
				message.setParam(&_spritesheet.get(_spriteGroupID));
				break;
			}
			case(MessageID::GET_SPRITE):
			{
				message.setParam(&_spritesheet.get(_spriteGroupID).get(_spriteID));
				break;
			}
			case(MessageID::SET_SPRITE_GROUP_ID):
			{
				_spriteGroupID = message.getParam<int>();
				break;
			}
			case(MessageID::SET_SPRITE_ID):
			{
				_spriteID = message.getParam<int>();
				break;
			}
			case(MessageID::DRAW):
			{
				draw();
				break;
			}
		}
	}
	
	void Renderer::draw(void) const
	{
		Message getPosition(MessageID::GET_POSITION);
		sendMessage(getPosition);
		const Vector& position = getPosition.getParam<Vector>();
		Message getOrientation(MessageID::GET_ORIENTATION);
		sendMessage(getOrientation);
		Orientation::Type spritesheetOrientation = _spritesheet.getOrientation();

		Orientation::Type orientation = getOrientation.isNullParam() ? spritesheetOrientation : getOrientation.getParam<Orientation::Type>();
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