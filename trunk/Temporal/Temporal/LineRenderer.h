#ifndef LINERENDERER_H
#define LINERENDERER_H
#include "NumericPair.h"
#include "Color.h"
#include "Component.h"

namespace Temporal
{
	class LineRenderer : public Component
	{
	public:
		LineRenderer(VisualLayer::Enum layer, Color color = Color::White) :
			_layer(layer), _target(Point::Zero), _color(color) {};

		ComponentType::Enum getType(void) const { return ComponentType::RENDERER; }

		void handleMessage(Message& message);
		void draw(void) const;
		Component* clone(void) const;
	private:
		const VisualLayer::Enum _layer;
		Point _target;
		Color _color;
	};
}
#endif