#ifndef LIGHTING_H
#define LIGHTING_H

#include "Color.h"
#include "EntitySystem.h"

namespace Temporal
{
	class Size;
	class Texture;

	class Light : public Component
	{
	public:
		ComponentType::Enum getType() const { return ComponentType::RENDERER; }
		void handleMessage(Message& message);
	private:
		int _radius;
		Color _color;

		void draw() const;
	};

	class LightSystem
	{
	public:
		void init(const Size& size);
		void preLightsDraw() const;
		void postLightsDraw() const;
		void postDraw() const;
	private:
		const Texture* _texture;
	};
}

#endif