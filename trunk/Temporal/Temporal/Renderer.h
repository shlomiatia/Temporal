#ifndef RENDERER_H
#define RENDERER_H
#include "Hash.h"
#include "Color.h"
#include "Component.h"

namespace Temporal
{
	class SceneNode;
	class SpriteSheet;

	class Renderer : public Component
	{
	public:
		Renderer(const SpriteSheet& spritesheet, VisualLayer::Enum layer, SceneNode* root, Color color = Color::White) :
		  _spritesheet(spritesheet), _layer(layer), _root(root), _color(color) {};

		ComponentType::Enum getType(void) const { return ComponentType::RENDERER; }
		SceneNode& getRoot(void) const { return *_root; }

		void handleMessage(Message& message);
		Component* clone(void) const;
	private:
		const SpriteSheet& _spritesheet;
		const VisualLayer::Enum _layer;

		SceneNode* _root;
		Color _color;

		void draw(void) const;

	};
}
#endif