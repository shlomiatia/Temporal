#ifndef LIGHTING_H
#define LIGHTING_H

#include "Color.h"
#include "EntitySystem.h"
#include "Math.h"
#include "Layer.h"

namespace Temporal
{
	class Size;
	class Texture;

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

	class Light : public Component
	{
	public:
		explicit Light(const Color& color = Color::White, float radius = 300.0f, float beamCenter = 0.0f, float beamSize = 2 * PI)
			: _color(color), _radius(radius), _beamCenter(beamCenter), _beamSize(beamSize) {}

		ComponentType::Enum getType() const { return ComponentType::RENDERER; }
		void handleMessage(Message& message);

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("color", _color);
			serializer.serialize("radius", _radius);
			serializer.serialize("center", _beamCenter);
			serializer.serialize("size", _beamSize);
		}
	private:
		Color _color;
		float _radius;
		float _beamCenter;
		float _beamSize;

		void draw() const;
	};

	class LightLayer : public Layer
	{
	public:
		LightLayer(const Color& ambientColor);
		void draw();

	private:
		const Color AMBIENT_COLOR;
		const Texture* _texture;

		void preDraw() const;
		void postDraw() const;

		LightLayer(const LightLayer&);
		LightLayer& operator=(const LightLayer&);
	};
}

#endif