#ifndef LIGHTING_H
#define LIGHTING_H

#include "Color.h"
#include "EntitySystem.h"
#include "Math.h"

namespace Temporal
{
	class Size;
	class Texture;

	class Light : public Component
	{
	public:
		Light(const Color& color, float radius, float beamCenter = 0.0f, float beamSize = 2 * PI)
			: _color(color), _radius(radius), _beamCenter(beamCenter), _beamSize(beamSize) {}

		ComponentType::Enum getType() const { return ComponentType::RENDERER; }
		void handleMessage(Message& message);
	private:
		const Color _color;
		const float _radius;
		const float _beamCenter;
		const float _beamSize;

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
		void preDraw() const;
		void postDraw() const;
	private:
		const Color AMBIENT_COLOR;
		const Texture* _texture;

		LightSystem()
			: AMBIENT_COLOR(0.1f, 0.1f, 0.1f) {}

		LightSystem(const LightSystem&);
		LightSystem& operator=(const LightSystem&);
	};
}

#endif