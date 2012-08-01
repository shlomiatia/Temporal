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
		Light(const Color& color, float radius)
			: _color(color), _radius(radius) {}

		ComponentType::Enum getType() const { return ComponentType::RENDERER; }
		void handleMessage(Message& message);
	private:
		Color _color;
		float _radius;

		void draw() const;
	};

	class LightGem : public Component
	{
	public:
		LightGem()
			: _isLit(false) {}

		ComponentType::Enum getType() const { return ComponentType::OTHER; }
		void handleMessage(Message& message);
	private:
		bool _isLit;
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