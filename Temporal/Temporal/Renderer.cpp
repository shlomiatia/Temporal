#include "Renderer.h"
#include "Graphics.h"
#include "SpriteSheet.h"
#include "Hash.h"
#include "Serialization.h"
#include "SceneNode.h"
#include "MessageUtils.h"
#include "ResourceManager.h"

namespace Temporal
{
	Renderer::~Renderer()
	{
		delete _root;
	}

	void Renderer::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_CREATED)
		{
			_spritesheet = ResourceManager::get().getSpritesheet(_spritesheetId);
			if(_sceneGraphId == Hash::INVALID)
				_root = new SceneNode(_sceneGraphId);
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

		Side::Enum spritesheetOrientation = _spritesheet->getOrientation();
		const Side::Enum entityOrientation = *static_cast<const Side::Enum*>(raiseMessage(Message(MessageID::GET_ORIENTATION)));

		_root->setMirrored(entityOrientation != spritesheetOrientation);
		_root->setTranslation(position);
		Graphics::get().draw(*_spritesheet, *_root, _color);
	}

	Component* Renderer::clone() const
	{
		return new Renderer(_spritesheetId, _sceneGraphId, _layer, _color);
	}

}