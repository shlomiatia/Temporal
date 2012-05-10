#ifndef RENDERER_H
#define RENDERER_H
#include <Temporal\Base\Hash.h>
#include <Temporal\Base\Color.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
	class SpriteSheet;

	class Renderer : public Component
	{
	public:
		Renderer(const SpriteSheet& spritesheet, VisualLayer::Enum layer, Color color = Color::White) :
		  _spritesheet(spritesheet), _layer(layer), _spriteGroupID(getFirstSpriteGroupID(spritesheet)), _spriteID(0), _color(color) {};

		ComponentType::Enum getType(void) const { return ComponentType::RENDERER; }

		void handleMessage(Message& message);
		void draw(void) const;
	private:
		const SpriteSheet& _spritesheet;
		const VisualLayer::Enum _layer;

		Hash _spriteGroupID;
		int _spriteID;
		Color _color;

		const Hash& getFirstSpriteGroupID(const SpriteSheet& spritesheet);
	};
}
#endif