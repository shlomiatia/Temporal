#include "Text.h"
#include "Layer.h"
#include "Vector.h"
#include "MessageUtils.h"
#include "ResourceManager.h"
#include <SDL_opengl.h>

namespace Temporal
{
	const Hash Text::TYPE = Hash("text");

	void Text::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			_font = ResourceManager::get().getFont(_fontFamily, _fontSize);
			_layout.SetFont(_font.get());
			//_layout->SetLineLength(64.0f);
		}
		else if(message.getID() == MessageID::DRAW)
		{
			LayerType::Enum layer = *static_cast<LayerType::Enum*>(message.getParam());
			if(layer == LayerType::GUI)
				draw();			
		}
	}
	
	void Text::draw()
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		Vector position = Vector::Zero;
		Message getDrawPosition(MessageID::GET_DRAW_POSITION, &position);
		raiseMessage(getDrawPosition);
		if(position == Vector::Zero)
			position = getPosition(*this);

		const Side::Enum entityOrientation = *static_cast<const Side::Enum*>(raiseMessage(Message(MessageID::GET_ORIENTATION)));

		glPushMatrix();
		{
			glTranslatef(position.getX(), position.getY(), 0.0f);
			_layout.Render(_text);
		}
		glPopMatrix();
	}

	Component* Text::clone() const
	{
		return new Text(_font, _text);
	}

}