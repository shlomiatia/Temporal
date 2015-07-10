#include "Renderer.h"
#include "Graphics.h"
#include "SpriteSheet.h"
#include "Hash.h"
#include "Serialization.h"
#include "SceneNode.h"
#include "MessageUtils.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Math.h"
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
				_spriteSheet = ResourceManager::get().getSingleTextureSpritesheet(_textureFile.c_str());
			if(!_root)
				_root = new SceneNode();
			_root->init();
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().getGameState().getLayersManager().removeSprite(_layer, this);
		}
		else if(message.getID() == MessageID::SET_COLOR)
		{
			const Color& color = *static_cast<Color*>(message.getParam());
			_color.setR(color.getR());
			_color.setG(color.getG());
			_color.setB(color.getB());

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
			Graphics::get().getMatrixStack().top().translate(position);
			if(entityOrientation != Side::RIGHT)
				Graphics::get().getMatrixStack().top().scale(Vector(-1.0f, 1.0f));
			
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
				Graphics::get().getMatrixStack().top().scale(Vector(-1.0f, 1.0f));
			Graphics::get().getMatrixStack().top().translate(sceneNode.getTranslation());
			Graphics::get().getMatrixStack().top().rotate(sceneNode.getRotation());
			Graphics::get().getMatrixStack().top().scale(sceneNode.getScale());

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
				Graphics::get().getSpriteBatch().add(&texture, Vector::Zero, texturePart, _color, 0.0f, -sprite.getOffset());
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