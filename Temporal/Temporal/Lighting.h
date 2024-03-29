#ifndef LIGHTING_H
#define LIGHTING_H

#include "Color.h"
#include "EntitySystem.h"
#include "Layer.h"
#include "FBO.h"
#include "SpriteBatch.h"
#include "ShaderProgram.h"
#include <memory>
#include "Ids.h"

namespace Temporal
{
	class SpriteSheet;

	class Light : public Component
	{
	public:
		explicit Light(const Color& color = Color::White, float radius = 256.0, float center = -3.14f / 2.0f, float size = 3.14f / 2.0f, bool activate = true)
			: _color(color), _radius(radius), _center(center), _size(size), _activate(activate) {}

		Hash getType() const { return ComponentsIds::LIGHT; }
		void handleMessage(Message& message);
		Component* clone() const { return new Light(_color, _radius, _center, _size, _activate); }
		
	private:
		std::shared_ptr<SpriteSheet> _lightTexture;
		std::shared_ptr<SpriteSheet> _shadowTexture;

		Color _color;
		float _radius;
		float _center;
		float _size;
		bool _activate;

		void draw();
		void drawBeamShadow(const Vector& lightCenter);
		void drawShadow(const Vector& lightCenter, const OBB& shape);
		void drawShadowPart(const Vector& lightCenter, const Vector& point1, const Vector& point2);

		friend class SerializationAccess;
	};

	class LightLayer : public Layer
	{
	public:
		LightLayer(LayersManager* manager, const Color& ambientColor);
		void add(Component* component) { _components.push_back(component); }
		void remove(Component* component);
		void draw(float framePeriod);
		ComponentList& get() { return _components; }

	private:
		const Color AMBIENT_COLOR;
		FBO _fbo;
		ComponentList _components;

		void preDraw();
		void postDraw();

		LightLayer(const LightLayer&);
		LightLayer& operator=(const LightLayer&);
	};
}
#endif