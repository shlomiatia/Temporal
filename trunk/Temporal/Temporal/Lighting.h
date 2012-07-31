#ifndef LIGHTING_H
#define LIGHTING_H

#include "Color.h"
#include "EntitySystem.h"

namespace Temporal
{
	class Size;
	class Texture;
	class Quad;

	typedef std::vector<const Quad*> QuadCollection;
	typedef QuadCollection::const_iterator QuadIterator;

	class Light : public Component
	{
	public:
		Light(const Color& color, float radius)
			: _color(color), _radius(radius) {}

		ComponentType::Enum getType() const { return ComponentType::RENDERER; }
		void handleMessage(Message& message);
	private:
		Color _color;
		float _radius;

		void draw() const;
	};

	class LightSystem
	{
	public:
		static LightSystem& get()
		{
			static LightSystem instance;
			return instance;
		}

		void init(const Size& size);
		void preLightsDraw() const;
		void postLightsDraw() const;
		void postDraw() const;
	private:
		const Texture* _texture;
	};
}

#endif