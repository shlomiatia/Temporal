#include "Text.h"
#include "Layer.h"
#include "Vector.h"
#include "MessageUtils.h"
#include "ResourceManager.h"
#include "Graphics.h"
#include <SDL_opengl.h>

namespace Temporal
{
	const Hash Text::TYPE = Hash("text");

	void Text::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			_font = ResourceManager::get().getFont(_fontFamily.c_str(), _fontSize);
			_layout.SetFont(_font.get());
		}
		else if(message.getID() == MessageID::SET_TEXT)
		{
			const char* text = static_cast<const char*>(message.getParam());
			_text = text;
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
		Vector position = getPosition(*this);
		glPushMatrix();
		{
			glTranslatef(position.getX(), position.getY(), 0.0f);
			_layout.Render(_text.c_str());
			Graphics::get().bindTexture(0); // Because we cache textures...
		}
		glPopMatrix();
	}

	Component* Text::clone() const
	{
		return new Text(_fontFamily.c_str(), _fontSize, _text.c_str());
	}

	void Text::setWidth(float width)
	{
		_layout.SetLineLength(width);
	}

	float Text::getWidth() const
	{
		return _layout.GetLineLength();
	}

	float Text::getHeight() const
	{
		FTBBox box =  const_cast<FTSimpleLayout&>(_layout).BBox(_text.c_str());
		return box.Upper().Yf() - box.Lower().Yf();
	}
}