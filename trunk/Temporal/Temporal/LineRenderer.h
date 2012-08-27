#ifndef LINERENDERER_H
#define LINERENDERER_H
#include "Vector.h"
#include "Color.h"
#include "EntitySystem.h"
#include "Layer.h"

namespace Temporal
{
	class Texture;

	class LineRenderer : public Component
	{
	public:
		LineRenderer(LayerType::Enum layer, const Texture& texture, Color color = Color::White) :
			_layer(layer), _target(Vector::Zero), _color(color), _texture(texture) {};

		ComponentType::Enum getType() const { return ComponentType::RENDERER; }

		void handleMessage(Message& message);
		void draw() const;
		Component* clone() const;
	private:
		const Texture& _texture;
		const LayerType::Enum _layer;
		Vector _target;
		Color _color;
	};
}
#endif