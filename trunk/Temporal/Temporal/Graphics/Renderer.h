#pragma once
#include <Temporal\Game\Component.h>
#include "SpriteSheet.h"

namespace Temporal
{
	class Renderer : public Component
	{
	public:
		Renderer(const SpriteSheet& spritesheet, VisualLayer::Enum layer, int spriteGroupID = 0, int spriteID = 0) : _spritesheet(spritesheet), _layer(layer), _spriteGroupID(spriteGroupID), _spriteID(spriteID) {};

		virtual ComponentType::Enum getType(void) const { return ComponentType::RENDERER; }

		virtual void handleMessage(Message& message);
		void draw(void) const;
	private:
		const SpriteSheet& _spritesheet;

		int _spriteGroupID;
		int _spriteID;
		VisualLayer::Enum _layer;

		Renderer(const Renderer&);
		Renderer& operator=(const Renderer&);
	};
}