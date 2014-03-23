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
			getEntity().getManager().getGameState().getLayersManager().addSprite(_layer, this);
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
			draw();
		}
		else if(message.getID() == MessageID::GET_ROOT_SCENE_NODE)
		{
			message.setParam(_root);
		}
	}
	
	void Renderer::draw()
	{
		const Vector& position = getPosition(*this);

		const Side::Enum entityOrientation = *static_cast<const Side::Enum*>(raiseMessage(Message(MessageID::GET_ORIENTATION)));

		
		Graphics::get().getMatrixStack().push();
		{
			Graphics::get().getMatrixStack().translate(position);
			if(entityOrientation != Side::RIGHT)
				Graphics::get().getMatrixStack().scale(Vector(-1.0f, 1.0f));
			draw(*_root);
		}
		Graphics::get().getMatrixStack().pop();
		Graphics::get().validate();
	}

	void Renderer::draw(const SceneNode& sceneNode)
	{
		Graphics::get().getMatrixStack().push();
		{	
			if(sceneNode.isFlip())
				Graphics::get().getMatrixStack().scale(Vector(-1.0f, 1.0f));
			Graphics::get().getMatrixStack().scale(sceneNode.getScale());
			Graphics::get().getMatrixStack().translate(sceneNode.getTranslation());
			Graphics::get().getMatrixStack().rotate(sceneNode.getRotation());

			for(SceneNodeIterator i = sceneNode.getChildren().begin(); i != sceneNode.getChildren().end(); ++i)
			{
				if((**i).drawBehindParent())
					draw(**i);
			}

			if(!sceneNode.isTransformOnly())
			{
				
				const Texture& texture = _spriteSheet->getTexture();
				Hash spriteGroupID = sceneNode.getSpriteGroupId();
				const SpriteGroup& spriteGroup = _spriteSheet->get(spriteGroupID);
				float spriteInterpolation = sceneNode.getSpriteInterpolation();
				int spriteIndex;
				if(spriteInterpolation < 0.0f)
					spriteIndex = 0;
				else if(spriteInterpolation >= 1.0f)
					spriteIndex = spriteGroup.getSize() - 1;
				else
					spriteIndex = static_cast<int>(spriteGroup.getSize() * sceneNode.getSpriteInterpolation());
				const Sprite& sprite = spriteGroup.get(spriteIndex);
				const AABB& texturePart = sprite.getBounds();
				Graphics::get().getMatrixStack().translate(-sprite.getOffset());
				Graphics::get().getSpriteBatch().add(&texture, texturePart, _color, Vector::Zero, Graphics::get().getMatrixStack().top());
			}

			for(SceneNodeIterator i = sceneNode.getChildren().begin(); i != sceneNode.getChildren().end(); ++i)
			{
				if(!(**i).drawBehindParent())
					draw(**i);
			}
		}
		Graphics::get().getMatrixStack().pop();
	}

	Component* Renderer::clone() const
	{
		SceneNode* sceneNodeClone = _root == 0 ? 0 : _root->clone();
		return new Renderer(_textureFile.c_str(), _spriteSheetFile.c_str(), sceneNodeClone, _layer, _color);
	}

}