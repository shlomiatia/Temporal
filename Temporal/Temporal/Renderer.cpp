#include "Renderer.h"
#include "Graphics.h"
#include "SpriteSheet.h"
#include "Hash.h"
#include "Serialization.h"
#include "SceneNode.h"
#include "MessageParams.h"

namespace Temporal
{
	void Renderer::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::SET_COLOR)
		{
			const Color& color = *static_cast<Color*>(message.getParam());
			_color.setColor(color);
		}
		else if(message.getID() == MessageID::SET_ALPHA)
		{
			float alpha = *static_cast<float*>(message.getParam());
			_color.setA(alpha);
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
		const Side::Enum entityOrientation = *(const Side::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));

		_root->setMirrored(entityOrientation != spritesheetOrientation);
		_root->setTranslation(position);
		Graphics::get().draw(_spritesheet, *_root, _color);
	}

	Component* Renderer::clone() const
	{
		return new Renderer(_spritesheet, _layer, _root->clone(), _color);
	}

}