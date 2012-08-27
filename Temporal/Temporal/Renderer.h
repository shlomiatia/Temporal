#ifndef RENDERER_H
#define RENDERER_H
#include "Hash.h"
#include "Color.h"
#include "EntitySystem.h"
#include "Layer.h"

namespace Temporal
{
	class SceneNode;
	class SpriteSheet;

	class Renderer : public Component
	{
	public:
		Renderer(const SpriteSheet& spritesheet, LayerType::Enum layer, SceneNode* root, Color color = Color::White) :
		  _spritesheet(spritesheet), _layer(layer), _root(root), _color(color) {};

		SceneNode& getRoot() const { return *_root; }
		ComponentType::Enum getType() const { return ComponentType::RENDERER; }

		void handleMessage(Message& message);
		Component* clone() const;
	private:
		const SpriteSheet& _spritesheet;
		const LayerType::Enum _layer;

		SceneNode* _root;
		Color _color;

		void draw() const;

	};
}
#endif