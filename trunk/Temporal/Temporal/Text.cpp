#include "Text.h"
#include "Layer.h"
#include "Vector.h"
#include "MessageUtils.h"
#include "ResourceManager.h"
#include <SDL_opengl.h>
#include <ftgl/ftgl.h>

namespace Temporal
{
	Text::Text(FTFont* font, char* text)
		: _font(font), _text(text) 
	{
		_layout = new FTSimpleLayout();
		_layout->SetFont(_font);
		//_layout->SetLineLength(50.0f);
	}
	void Text::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			_font = ResourceManager::get().getFont(_fontFamily, _fontSize);
		}
		else if(message.getID() == MessageID::DRAW)
		{
			LayerType::Enum layer = *static_cast<LayerType::Enum*>(message.getParam());
			if(layer == LayerType::GUI)
				draw();			
		}
	}
	
	void Text::draw() const
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
			//_font->Render(_text);
			FT_Error error = _layout->Error();
			_layout->Render(_text);
		}
		glPopMatrix();
	}

	Component* Text::clone() const
	{
		return new Text(_font, _text);
	}

}