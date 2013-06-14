#include "Renderer.h"
#include "Graphics.h"
#include "SpriteSheet.h"
#include "Hash.h"
#include "Serialization.h"
#include "SceneNode.h"
#include "MessageUtils.h"
#include "ResourceManager.h"
#include <SDL_opengl.h>

namespace Temporal
{
	Renderer::Renderer(SceneNode* root, LayerType::Enum layer, Color color) :
		_root(root), _layer(layer), _color(color) 
	{
	}

	Renderer::~Renderer()
	{
		delete _root;
	}

	void Renderer::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			_spriteSheet = ResourceManager::get().getSpritesheet(_spriteSheetFile);
			if(!_root)
				_root = new SceneNode();
		}
		else if(message.getID() == MessageID::SET_COLOR)
		{
			const Color& color = *static_cast<Color*>(message.getParam());
			_color.setColor(color);
		}
		else if(message.getID() == MessageID::SET_ALPHA)
		{
			float alpha = getFloatParam(message.getParam());
			_color.setA(alpha);
		}
		else if(message.getID() == MessageID::DRAW)
		{
			LayerType::Enum layer = *static_cast<LayerType::Enum*>(message.getParam());
			if(_layer == layer)
				draw();			
		}
	}
	
	void Renderer::draw() const
	{
		Vector position = Vector::Zero;
		Message getDrawPosition(MessageID::GET_DRAW_POSITION, &position);
		raiseMessage(getDrawPosition);
		if(position == Vector::Zero)
			position = getPosition(*this);

		const Side::Enum entityOrientation = *static_cast<const Side::Enum*>(raiseMessage(Message(MessageID::GET_ORIENTATION)));

		glPushMatrix();
		{
			glTranslatef(position.getX(), position.getY(), 0.0f);
			if(entityOrientation != Side::RIGHT)
				glScalef(-1.0f, 1.0f, 1.0f);
			Graphics::get().draw(*_root, *_spriteSheet.get(), _color);
		}
		glPopMatrix();
	}

	Component* Renderer::clone() const
	{
		return new Renderer(_root->clone(), _layer, _color);
	}

}