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
	const Hash Renderer::TYPE = Hash("renderer");

	Renderer::Renderer(const char* textueFile, const char* spritesheetFile, SceneNode* root, LayerType::Enum layer, Color color) :
		_spriteSheetFile(spritesheetFile), _textureFile(textueFile), _spriteSheet(0), _root(root), _layer(layer), _color(color) 
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
			if(!_spriteSheetFile.empty())
				_spriteSheet = ResourceManager::get().getSpritesheet(_spriteSheetFile.c_str());
			else
				_spriteSheet = ResourceManager::get().getTexture(_textureFile.c_str());
			if(!_root)
				_root = new SceneNode();
		}
		else if(message.getID() == MessageID::SET_COLOR)
		{
			const Color& color = *static_cast<Color*>(message.getParam());
			_color = color;
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
		SceneNode* sceneNodeClone = _root == 0 ? 0 : _root->clone();
		return new Renderer(_textureFile.c_str(), _spriteSheetFile.c_str(), sceneNodeClone, _layer, _color);
	}

}